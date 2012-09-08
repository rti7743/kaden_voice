// HttpServer.h: HttpServer クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HttpServer_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_)
#define AFX_HttpServer_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum WEBSERVER_RESULT_TYPE
{
	 WEBSERVER_RESULT_TYPE_OK_HTML
	,WEBSERVER_RESULT_TYPE_OK_JSON
	,WEBSERVER_RESULT_TYPE_OK_DATA
	,WEBSERVER_RESULT_TYPE_TRASMITFILE
	,WEBSERVER_RESULT_TYPE_ERROR
	,WEBSERVER_RESULT_TYPE_NOT_FOUND
	,WEBSERVER_RESULT_TYPE_LOCATION
	,WEBSERVER_RESULT_TYPE_FORBIDDEN
};
class HttpLongPoolingInterface
{
public:
	HttpLongPoolingInterface(){};
	virtual ~HttpLongPoolingInterface(){};
	virtual bool Pooling(int count);
	virtual std::string getFulltext() const;
};


class HttpWorker
{
	MainWindow* PoolMainWindow;
	boost::asio::ip::tcp::socket* ConnectSocket;
public:
	HttpWorker(MainWindow* poolMainWindow, boost::asio::ip::tcp::socket* socket);
	virtual ~HttpWorker();

	void operator()();
private:
	void HTTP500();
	void HTTP404();
	void HTTP403();
	void HTTP302(const std::string& url);
	void HTTP200(const std::string& contents,const std::string& headers);
	void HTTP200SendFileContent(const std::string& urlpath);
	void HTTP200LongPooling(const std::string& headers,HttpLongPoolingInterface* longPoolingObhect);

	bool ProcToken(const std::map<std::string,std::string>& header,const std::map<std::string,std::string>& request) const;
};

class HttpServer
{
    boost::thread*					Thread;
public:

	HttpServer();
	virtual ~HttpServer();
	void Create(MainWindow* poolMainWindow,int port,int threadcount,const std::string& webroot,const std::map<std::string,std::string>& allowext);
	void stop();
	bool Regist(const CallbackDataStruct * callback ,const std::string & path);
	bool RemoveCallback(const CallbackDataStruct* callback , bool is_unrefCallback) ;

	std::string getAllowExtAndMime(const std::string& ext) const;
	std::string getWebURL(const std::string& path) const;
	std::string WebPathToRealPath(const std::string& urlpath);
	//サーバのトップアドレス.
	std::string getServerTop() const;

	std::string getWebroot() const
	{
		return this->Webroot;
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
	int Port;
	std::map<std::string,std::string> AllowExtAndMime;
	mutable boost::mutex lock;
};


#endif // !defined(AFX_HttpServer_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_)
