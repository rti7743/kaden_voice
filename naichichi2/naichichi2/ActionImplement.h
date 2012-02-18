#pragma once
class ActionImplement
{
public:
	ActionImplement(void);
	~ActionImplement(void);

	static xreturn::r<std::string> ActionImplement::Telnet(const std::string& host,int port,const std::string& wait,const std::string& send,const std::string& recv);
	static xreturn::r<std::string> ActionImplement::HttpGet(const std::string& url);
	static xreturn::r<std::string> ActionImplement::HttpPost(const std::string& url,const std::string& postdata);
	static xreturn::r<bool> ActionImplement::Execute(const std::string& targetpc,const std::string& command,const std::string& args,const std::string& directory);
	static xreturn::r<bool> ActionImplement::SendKeydown(const std::string& targetpc,const std::string& windowname,int key , int keyoption);
	static xreturn::r<bool> ActionImplement::SendMessage(const std::string& targetpc,const std::string& windowname,int message,int wparam,int  lparam);
	static xreturn::r<bool> ActionImplement::MSleep(const std::string& targetpc,unsigned int mtime);

};

