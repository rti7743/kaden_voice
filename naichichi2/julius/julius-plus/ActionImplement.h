#pragma once
#include "xreturn.h"

class ActionImplement
{
public:
	ActionImplement(void);
	~ActionImplement(void);

	static xreturn::r<std::string> Telnet(const std::string& host,const std::string& port,const std::string& wait,const std::string& send,const std::string& recv);

};

