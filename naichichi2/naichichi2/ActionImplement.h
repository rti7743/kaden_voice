#pragma once
class ActionImplement
{
public:
	ActionImplement(void);
	~ActionImplement(void);

	static std::string Telnet(const std::string& host,int port,const std::string& wait,const std::string& send,const std::string& recv);
	static std::string HttpGet(const std::string& url);
	static std::string HttpPost(const std::string& url,const std::string& postdata);
	static bool Execute(const std::string& targetpc,const std::string& command,const std::string& args);
	static bool OpenWeb(const std::string& targetpc,const std::string& command);
	static bool SendKeydown(const std::string& targetpc,const std::string& windowname,int key , int keyoption);
	static bool SendMessage(const std::string& targetpc,const std::string& windowname,int message,int wparam,int  lparam);
	static bool MSleep(const std::string& targetpc,unsigned int mtime);

};

