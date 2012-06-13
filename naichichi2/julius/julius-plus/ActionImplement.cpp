#ifdef _MSC_VER
#include <Winsock2.h>
#include <mswsock.h>

#include <windows.h>

 // This check can be removed if you only build for Windows
#pragma warning (push)
#pragma warning (disable : 4005)
#include <intsafe.h>
#include <stdint.h>
#pragma warning (push)
#pragma warning (default : 4005)
#endif //_MSC_VER

#include <fstream>
#include <list>
#include <vector>
#include <iostream>
#include <sstream>
#include "ActionImplement.h"
#include <boost/asio.hpp>

ActionImplement::ActionImplement(void)
{
}


ActionImplement::~ActionImplement(void)
{
}

xreturn::r<std::string> ActionImplement::Telnet(const std::string& host,const std::string& port,const std::string& wait,const std::string& send,const std::string& recv)
{
	//ASIO作成
	boost::asio::io_service io_service;

	 //名前解決
	boost::asio::ip::tcp::resolver resolver(io_service);
	boost::asio::ip::tcp::resolver::query query(host ,  port );
	boost::asio::ip::tcp::endpoint endpoint(*resolver.resolve(query));

    boost::asio::ip::tcp::socket socket(io_service);

	//接続
	boost::system::error_code error ;
	socket.connect(endpoint,error);
    if (error)
	{
		return xreturn::error("ホストに接続できません host:" + host + " port:" + port );
	}

	//まずは相手の言い分を聞きます.
	if (!wait.empty())
	{
		boost::asio::streambuf wait_body;
		boost::asio::read_until(socket, wait_body,wait ,error );
	}

	//送信
	if (!send.empty() )
	{
		boost::asio::streambuf request;
		std::ostream request_stream(&request);
		request_stream << send << std::flush;
		boost::asio::write(socket, request);
	}

	//最後に会いての言い分を聞いてから通信を切ります。
	if (!recv.empty())
	{
		//貴方の意見は？
		boost::asio::streambuf respons_body;
		boost::asio::read_until(socket, respons_body,recv ,error );
//		return boost::asio::buffer_cast<const char*>(respons_body.data());
	}
	return std::string("");
}
