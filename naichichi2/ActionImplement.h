#pragma once
class ActionImplement
{
public:
	ActionImplement(void);
	~ActionImplement(void);

	static xreturn::r<std::string> Telnet(const std::string& host,int port,const std::string& wait,const std::string& send,const std::string& recv);
	static xreturn::r<std::string> HttpGet(const std::string& url);
	static xreturn::r<std::string> HttpPost(const std::string& url,const std::string& postdata);
	static xreturn::r<bool> Execute(const std::string& targetpc,const std::string& command,const std::string& args);
	static xreturn::r<bool> OpenWeb(const std::string& targetpc,const std::string& command);
	static xreturn::r<bool> SendKeydown(const std::string& targetpc,const std::string& windowname,int key , int keyoption);
	static xreturn::r<bool> SendMessage(const std::string& targetpc,const std::string& windowname,int message,int wparam,int  lparam);
	static xreturn::r<bool> MSleep(const std::string& targetpc,unsigned int mtime);

};

