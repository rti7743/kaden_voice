// HttpServer.h: HttpServer クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HttpServer_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_)
#define AFX_HttpServer_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class HttpServer;

class HttpWorker
{
	HttpServer * PoolServer;
	boost::asio::ip::tcp::socket* ConnectSocket;
public:
	HttpWorker(HttpServer * poolServer, boost::asio::ip::tcp::socket* socket);
	virtual ~HttpWorker();

	void operator()();
private:
	void HttpWorker::HTTP500();
	void HttpWorker::HTTP404();
	void HttpWorker::HTTP403();
	void HttpWorker::HTTP302(const std::string& url);
	void HttpWorker::HTTP200(const std::string& contents);
	void HttpWorker::HTTP200SendFileContent(const std::string& urlpath);
	bool HttpWorker::ProcToken(const std::map<std::string,std::string>& header,const std::map<std::string,std::string>& request) const;
};

class HttpServer
{
    boost::thread*					Thread;
public:

	HttpServer::HttpServer();
	virtual HttpServer::~HttpServer();
	void HttpServer::Create(MainWindow* poolMainWindow,int port,int threadcount,const std::string& webroot,const std::string& accesstoken,const std::map<std::string,std::string>& allowext);
	void HttpServer::stop();
	xreturn::r<bool> HttpServer::Regist(CallbackDataStruct & callback ,const std::string & path);
	bool HttpServer::FireCallback(const std::string & path,const std::map<std::string,std::string> & request,std::string * respons,WEBSERVER_RESULT_TYPE* type) const;
	std::string HttpServer::getAllowExtAndMime(const std::string& ext) const;
	std::string HttpServer::getWebURL(const std::string& path) const;
	bool HttpServer::checkAccessToken(const std::string& token) const;
	std::string HttpServer::WebPathToRealPath(const std::string& urlpath);

	std::string getWebroot() const
	{
		return this->Webroot;
	}
	std::string getAccesstoken() const
	{
		return this->Accesstoken;
	}
private:
	void HttpServer::acceptThread(int threadcount);

	boost::asio::io_service AcceptorIOService;
	boost::asio::ip::tcp::acceptor* Acceptor;
	std::map<std::string,CallbackDataStruct> CallbackDictionary;
	MainWindow * PoolMainWindow;

	bool StopFlag;
	std::string Webroot;
	std::string Accesstoken;
	int Port;
	std::map<std::string,std::string> AllowExtAndMime;
	mutable boost::mutex lock;
};


#endif // !defined(AFX_HttpServer_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_)
