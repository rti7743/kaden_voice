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

HttpWorker::HttpWorker(HttpServer * poolServer, boost::asio::ip::tcp::socket* socket) : PoolServer(poolServer),ConnectSocket(socket)
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
void HttpWorker::HTTP200(const std::string& contents,const std::string& headers)
{
    boost::asio::streambuf respons;
    std::ostream respons_stream(&respons);
	respons_stream <<	"HTTP/1.0 200 OK\r\n"
						"Pragma:no-cache\r\n"
						"Server:naichichi2\r\n"
						"Content-Length: " << contents.size() << "\r\n"
						<< headers << "\r\n"
						<< contents 
						<< std::flush;

	boost::system::error_code ec;
	boost::asio::write(*this->ConnectSocket,respons ,ec);
}

void HttpWorker::HTTP200SendFileContent(const std::string& urlpath)
{
	//アクセスが許可されている拡張子か？
	std::string mime = this->PoolServer->getAllowExtAndMime( XLStringUtil::strtolower(XLStringUtil::baseext_nodot(urlpath)) );
	if (mime.empty())
	{
		HTTP403();
		return;
	}

	//ファイルがあるか？
	std::string realpath = this->PoolServer->WebPathToRealPath(urlpath);
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
	std::string sendstring;
	XLHttpHeader httpHeaders;
	if ( ! httpHeaders.Parse( requestdata ) )
	{
		HTTP500();
		return ;
	}

	std::string path = httpHeaders.getRequestPath();
	std::map<std::string,std::string> header = httpHeaders.getHeader();
	std::map<std::string,std::string> request = httpHeaders.getRequest();

	//イタヅラ防止のトークンキーを取得する.
	if ( ! ProcToken(header,request) )
	{//トークンが合わない
		HTTP500();
		return ;
	}

	//lua処理系に投げる
	std::string responsString;
	WEBSERVER_RESULT_TYPE type;
	std::string headers;
	bool r = this->PoolServer->FireCallback(path,request,&responsString, &type,&headers);
	if (!r)
	{//luaにないらしい、画像などのリアルコンテンツ？
		HTTP200SendFileContent(path);
		return ;
	}

	switch(type)
	{
	case WEBSERVER_RESULT_TYPE_OK:
		HTTP200(responsString,headers);
		return ;
	case WEBSERVER_RESULT_TYPE_TRASMITFILE:
		HTTP200SendFileContent(responsString);
		return ;
	case WEBSERVER_RESULT_TYPE_ERROR:
		HTTP500();
		return ;
	case WEBSERVER_RESULT_TYPE_NOT_FOUND:
		HTTP404();
		return ;
	case WEBSERVER_RESULT_TYPE_LOCATION:
		HTTP302(responsString);
		return ;
	};

	this->ConnectSocket->close();
}

bool HttpWorker::ProcToken(const std::map<std::string,std::string>& header,const std::map<std::string,std::string>& request) const
{
	//内部からのリファラーを持つアクセスだったら許可する。
	//理由:ブラウザからのXSSを防止するのが目的なのでこれで大丈夫だと思われる
	{
		auto it = header.find("Referer");
		if (it == header.end())
		{
			it = header.find("referer");
		}
		if (it != header.end())
		{
			if ( it->second.find( this->PoolServer->getServerTop() ) == 0 )
			{
				//OK 内部からのアクセスだ。
				return true;
			}
		}
	}

	//それ以外はアクセストークンを確認する.
	{
		auto it = request.find("accesstoken");
		if (it != request.end())
		{
			if (this->PoolServer->checkAccessToken(it->second) )
			{//OK 正しいトークンだ
				return true;
			}
		}
	}

	//アクセス拒否
	return false;
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
		ioWorker.post([=](){ HttpWorker w(this,socket); w(); delete socket; });
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



bool HttpServer::FireCallback(const std::string & path,const std::map<std::string,std::string> & request,std::string * respons,WEBSERVER_RESULT_TYPE* type,std::string* headers) const
{
	const CallbackDataStruct* callback;
	//対応するコールバックルーチンを取得する.
	{
		//ロックを最短にしたい。
		boost::unique_lock<boost::mutex> al(this->lock);

		auto it = this->CallbackDictionary.find(path);
		if (it == this->CallbackDictionary.end() )
		{
			*type = WEBSERVER_RESULT_TYPE_NOT_FOUND;
			*respons = "";
			return false;
		}
		callback = it->second;
	}

	this->PoolMainWindow->SyncInvokeLog(std::string() + "webから実行 " + path ,LOG_LEVEL_DEBUG);

	//メインスレッドで動いている シナリオを呼び出す.
	this->PoolMainWindow->SyncInvoke( [&](){
		try
		{
			ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません
			this->PoolMainWindow->ScriptManager.HttpRequest( callback ,path,request, respons ,  type,headers);
		}
		catch(xreturn::error &e)
		{
			this->PoolMainWindow->SyncInvokeError(e.getFullErrorMessage());
		}
	} );
	return true;
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
SEXYTEST("HttpServer::WebPathToRealPath")
{
	
	{
		HttpServer server;
		server.Webroot = "./config/webroot";

		std::string a = server.WebPathToRealPath("./hello.tpl");
		SEXYTEST_EQ(a ,".\\config\\webroot\\hello.tpl");
	}
}

xreturn::r<bool> HttpServer::RemoveCallback(const CallbackDataStruct* callback , bool is_unrefCallback) 
{
	boost::unique_lock<boost::mutex> al(this->lock);

	CRemoveIF(this->CallbackDictionary , {
		if (_.second == callback)
		{
			return false; //消す.
		}
	});
	return true;
}
