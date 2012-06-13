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
	void HTTP500();
	void HTTP404();
	void HTTP403();
	void HTTP302(const std::string& url);
	void HTTP200(const std::string& contents,const std::string& headers);
	void HTTP200SendFileContent(const std::string& urlpath);
	bool ProcToken(const std::map<std::string,std::string>& header,const std::map<std::string,std::string>& request) const;
};

class HttpServer
{
    boost::thread*					Thread;
public:

	HttpServer();
	virtual ~HttpServer();
	void Create(MainWindow* poolMainWindow,int port,int threadcount,const std::string& webroot,const std::string& accesstoken,const std::map<std::string,std::string>& allowext);
	void stop();
	xreturn::r<bool> Regist(const CallbackDataStruct * callback ,const std::string & path);
	xreturn::r<bool> RemoveCallback(const CallbackDataStruct* callback , bool is_unrefCallback) ;

	bool FireCallback(const std::string & path,const std::map<std::string,std::string> & request,std::string * respons,WEBSERVER_RESULT_TYPE* type,std::string* headers) const;
	std::string getAllowExtAndMime(const std::string& ext) const;
	std::string getWebURL(const std::string& path) const;
	bool checkAccessToken(const std::string& token) const;
	std::string WebPathToRealPath(const std::string& urlpath);
	//サーバのトップアドレス. (accesstokenキーなし)
	std::string getServerTop() const;

	std::string getWebroot() const
	{
		return this->Webroot;
	}
	std::string getAccesstoken() const
	{
		return this->Accesstoken;
	}

	SEXYTEST_TEST_FRIEND;
private:
	void acceptThread(int threadcount);

	boost::asio::io_service AcceptorIOService;
	boost::asio::ip::tcp::acceptor* Acceptor;
	std::map<std::string,const CallbackDataStruct*> CallbackDictionary;
	MainWindow * PoolMainWindow;

	bool StopFlag;
	std::string Webroot;
	std::string Accesstoken;
	int Port;
	std::map<std::string,std::string> AllowExtAndMime;
	mutable boost::mutex lock;
};


#endif // !defined(AFX_HttpServer_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_)
