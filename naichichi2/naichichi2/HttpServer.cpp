// HttpServer.cpp: HttpServer クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "common.h"
#include "ScriptRunner.h"
#include "HttpServer.h"
#include "MainWindow.h"
#include "XLHttpHeader.h"
#include "XLStringUtil.h"
#include "XLFileUtil.h"

HttpWorker::HttpWorker(MainWindow* poolMainWindow,boost::asio::ip::tcp::socket* socket) : PoolMainWindow(poolMainWindow) ,ConnectSocket(socket)
{
}
HttpWorker::~HttpWorker()
{
}

void HttpWorker::HTTP500()
{
    boost::asio::streambuf respons;
    std::ostream respons_stream(&respons);
	respons_stream <<	"HTTP/1.0 500 Internal Server Error\r\n"
						"Pragma:no-cache\r\n"
						"Server:naichichi2\r\n"
						"\r\n"
						<< std::flush;

	boost::system::error_code ec;
	boost::asio::write(*this->ConnectSocket,respons ,ec);
}
void HttpWorker::HTTP404()
{
    boost::asio::streambuf respons;
    std::ostream respons_stream(&respons);
	respons_stream <<	"HTTP/1.0 404 Not Found\r\n"
						"Pragma:no-cache\r\n"
						"Server:naichichi2\r\n"
						"\r\n"
						<< std::flush;

	boost::system::error_code ec;
	boost::asio::write(*this->ConnectSocket,respons ,ec);
}

void HttpWorker::HTTP403()
{
    boost::asio::streambuf respons;
    std::ostream respons_stream(&respons);
	respons_stream <<	"HTTP/1.0 403 Forbidden\r\n"
						"Pragma:no-cache\r\n"
						"Server:naichichi2\r\n"
						"\r\n"
						<< std::flush;

	boost::system::error_code ec;
	boost::asio::write(*this->ConnectSocket,respons ,ec);
}



void HttpWorker::HTTP302(const std::string& url)
{
    boost::asio::streambuf respons;
    std::ostream respons_stream(&respons);
	respons_stream <<	"HTTP/1.0 302 Found\r\n"
						"Pragma:no-cache\r\n"
						"Server:naichichi2\r\n"
						"Location: " << XLStringUtil::urlencode(url) << "\r\n"
						"\r\n"
						<< std::flush;

	boost::system::error_code ec;
	boost::asio::write(*this->ConnectSocket,respons ,ec);
}
void HttpWorker::HTTP200(const std::string& contents,const std::string& headers,bool texthtml)
{
    boost::asio::streambuf respons;
    std::ostream respons_stream(&respons);
	respons_stream <<	"HTTP/1.0 200 OK\r\n"
						"Pragma:no-cache\r\n"
						"Server:naichichi2\r\n"
						"Content-Length: " << contents.size() << "\r\n";
	if (texthtml)
	{
		respons_stream << "Content-type: text/html; charset=UTF-8\r\n";
	}
	respons_stream		<< headers << "\r\n"
						<< contents 
						<< std::flush;

	boost::system::error_code ec;
	boost::asio::write(*this->ConnectSocket,respons ,ec);
}


void HttpWorker::HTTP200SendFileContent(const std::string& urlpath)
{
	//アクセスが許可されている拡張子か？
	std::string mime = this->PoolMainWindow->Httpd.getAllowExtAndMime( XLStringUtil::strtolower(XLStringUtil::baseext_nodot(urlpath)) );
	if (mime.empty())
	{
		HTTP403();
		return;
	}

	//ファイルがあるか？
	std::string realpath = this->PoolMainWindow->Httpd.WebPathToRealPath(urlpath);
	if (realpath.empty())
	{
		HTTP404();
		return ;
	}

	//実際の転送(transmitfile / sendfileにしたいね)
	std::vector<char> filebinary = XLFileUtil::cat_b(realpath);
	if (filebinary.empty())
	{
		HTTP403();
		return;
	}

    boost::asio::streambuf respons;
    std::ostream respons_stream(&respons);
	respons_stream <<	"HTTP/1.0 200 OK\r\n"
						"Pragma:no-cache\r\n"
						"Server:naichichi2\r\n"
						"Content-Length: " << filebinary.size() << "\r\n"
						"Content-Type: " << mime << "\r\n"
						"\r\n"
						<< std::flush;
	boost::system::error_code ec;
	boost::asio::write(*this->ConnectSocket,respons ,ec);
	boost::asio::write(*this->ConnectSocket,boost::asio::buffer(&filebinary[0],filebinary.size() ) ,ec);
}


void HttpWorker::operator()()
{
//	assert( this->ConnectSocket->is_open() );
	boost::asio::streambuf response_body;
	boost::system::error_code error ;
	//とりあえず今はこれでいい。タイムアウトの設定ができていないので、DoSに弱い。
	//あとでboost捨てよう。自前に置き換えてしまおう。
	boost::asio::read_until(*this->ConnectSocket, response_body,"\r\n\r\n" ,error );
	if (error)
	{
		return ;
	}

	//ヘッダーパース
	const char* requestdata = boost::asio::buffer_cast<const char*>(response_body.data());
	int size = response_body.size();
	std::string sendstring;
	XLHttpHeader httpHeaders;
	if ( ! httpHeaders.Parse( requestdata ,size) )
	{
		HTTP500();
		return ;
	}

	//コンテンツを受け取る.
	if (httpHeaders.getRequestMethod() == "POST")
	{
		std::map<std::string,std::string>::const_iterator it;
		const auto header = httpHeaders.getHeaderPointer();
		if ( (it = header->find("content-length")) != header->end() )
		{
			unsigned int contentSize = atoi(it->second.c_str());
			while(response_body.size() < httpHeaders.getHeaderSize() + contentSize )
			{
				boost::asio::read(*this->ConnectSocket, response_body
					,boost::asio::transfer_at_least(1) ,error );
				if (error)
				{
					break ;
				}
			}

			if (response_body.size() < httpHeaders.getHeaderSize() + contentSize)
			{
				HTTP403();
				return ;
			}
			requestdata = boost::asio::buffer_cast<const char*>(response_body.data());
			httpHeaders.PostParse(requestdata+httpHeaders.getHeaderSize() ,contentSize );
		}
	}

	//パスはよく使うので変数に入れるよ
	const std::string path = httpHeaders.getRequestPath();

	//処理系に投げる
	std::string responsString;
	std::string headers;
	WEBSERVER_RESULT_TYPE result = WEBSERVER_RESULT_TYPE_NOT_FOUND;
	if (!  this->PoolMainWindow->ScriptManager.WebAccess(path,httpHeaders,&result,&responsString) )
	{
		HTTP200SendFileContent(path);
		this->ConnectSocket->close();
		return ;
	}

	switch(result)
	{
	case WEBSERVER_RESULT_TYPE_OK:
		HTTP200(responsString,headers,true);
		break;
	case WEBSERVER_RESULT_TYPE_TRASMITFILE:
		HTTP200SendFileContent(responsString);
		break;
	case WEBSERVER_RESULT_TYPE_ERROR:
		HTTP500();
		break;
	case WEBSERVER_RESULT_TYPE_LOCATION:
		HTTP302(responsString);
		break;
	case WEBSERVER_RESULT_TYPE_NOT_FOUND:
		HTTP404();
		break;
	};

	this->ConnectSocket->close();
}

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////
HttpServer::HttpServer()
{
	this->Thread = NULL;
	this->StopFlag = false;
}
HttpServer::~HttpServer()
{
	stop();
}
void HttpServer::Create(MainWindow* poolMainWindow,int port,int threadcount,const std::string& webroot,const std::string& accesstoken,const std::map<std::string,std::string>& allowext)
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません

	this->PoolMainWindow = poolMainWindow;
	this->Webroot = webroot;
	this->Port = port;
	this->Accesstoken = accesstoken;
	this->AllowExtAndMime = allowext;

	//終了時にこれをcloseしてやらんと無限ループるみたい。めんどい。
	this->Acceptor = new boost::asio::ip::tcp::acceptor(  this->AcceptorIOService
		 , boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)
	);
	//acceptスレッド作成.
	this->Thread = new boost::thread([=](){
		try
		{
			this->acceptThread(threadcount); 
		}
		catch(xreturn::error & e)
		{
			this->PoolMainWindow->SyncInvokeError(e.getFullErrorMessage());
		}
	});
}

void HttpServer::stop()
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません

	if (this->Thread)
	{
		this->StopFlag = true;
		this->AcceptorIOService.stop();
		this->Acceptor->close();
		this->Thread->join();

		delete this->Thread;
		this->Thread = NULL;

		delete this->Acceptor;
		this->Acceptor = NULL;
	}
}

void HttpServer::acceptThread(int threadcount)
{
	boost::asio::io_service ioWorker;
	boost::asio::io_service::work w( ioWorker );

	boost::thread_group tg;
	for( int i=0; i<threadcount; ++i )
	{
		tg.create_thread( boost::bind( &boost::asio::io_service::run, &ioWorker ) );
	}

	while (!this->StopFlag)
	{
		//accept のエラーをチェックしていないで、 終了時の close()すると例外で死ぬ。

		boost::system::error_code ec;
		// 接続待ち
		boost::asio::ip::tcp::socket* socket = new boost::asio::ip::tcp::socket(this->AcceptorIOService);
		this->Acceptor->accept( *socket ,ec);
		if (this->StopFlag)
		{
			delete socket;
			break;
		}

		//ワーカースレッドになげる.
		ioWorker.post([=](){ HttpWorker w(this->PoolMainWindow,socket); w(); delete socket; });
	}
	ioWorker.stop();
	tg.join_all();
}

xreturn::r<bool>  HttpServer::Regist(const CallbackDataStruct * callback ,const std::string & path)
{
	boost::unique_lock<boost::mutex> al(this->lock);

	this->CallbackDictionary.insert( std::pair<std::string,const CallbackDataStruct*>(path, callback) );
	return true;
}

std::string HttpServer::getAllowExtAndMime(const std::string& ext) const
{
	auto it = this->AllowExtAndMime.find(ext);
	if (it == this->AllowExtAndMime.end() )
	{
		return "";
	}
	return it->second;
}

//続きはwebで
std::string HttpServer::getWebURL(const std::string& path) const
{
	std::string url = this->getServerTop() ;

	if (path.empty())        url += "/";
	else if (path[0] != '/') url += "/" + path;
	else url += path;

	return XLStringUtil::AppendURLParam(url,"accesstoken=" + this->Accesstoken);
}

//サーバのトップアドレス. (accesstokenキーなし)
std::string HttpServer::getServerTop() const
{
	return "http://127.0.0.1:" + num2str(this->Port) ;
}

bool HttpServer::checkAccessToken(const std::string& token) const
{
	return token == this->Accesstoken;
}

std::string HttpServer::WebPathToRealPath(const std::string& urlpath)
{
	//パスの区切りを / で統一します。
	const std::string _webroot = XLStringUtil::replace(this->Webroot , "\\" , "/");
	std::string realpath = XLStringUtil::pathcombine("./",_webroot + "\\" + urlpath,"/");

	//webrootより上にアクセスしていないか？
	if (realpath.find(_webroot) != 0)
	{
		return "";
	}

	//パス区切り文字をOS標準に再度合わせます。
	realpath = XLStringUtil::pathseparator(realpath);
	return realpath;
}

/*
SEXYTEST(HttpServer__WebPathToRealPath,"HttpServer::WebPathToRealPath")
{
	
	{
		HttpServer server;
		server.Webroot = "./config/webroot";

		std::string a = server.WebPathToRealPath("./hello.tpl");
		SEXYTEST_EQ(a ,".\\config\\webroot\\hello.tpl");
	}
}
*/


xreturn::r<bool> HttpServer::RemoveCallback(const CallbackDataStruct* callback , bool is_unrefCallback) 
{
	boost::unique_lock<boost::mutex> al(this->lock);
	return true;
}
