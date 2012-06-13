// XLUrlParser.cpp: XLUrlParser クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "common.h"
#include "XLUrlParser.h"

//USE_TEST(XLUrlParser)


//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

XLUrlParser::XLUrlParser()
{

}

XLUrlParser::~XLUrlParser()
{

}

void XLUrlParser::Parse(const std::string & inURL)
{
	this->Protocol = "http";
	this->UserName = "";
	this->Password = "";
	this->Host = "";
	this->Port = -1;
	this->Path = "/";

	const char * url = inURL.c_str();

	const char * slash = strstr(url , "/");

	//プロトコル
	const char * slashslash = strstr(url , "://");
	if (slashslash != NULL)
	{
		this->Protocol = std::string(url ,  0 , slashslash - url );
		url = slashslash + 3;
	}

	//ユーザ名、パスワード、ホスト名とポート番号の部分だけを抜き出す
	std::string host;
	const char * slash2 = strstr(url , "/");
	if (slash2 == NULL)
	{
		host = url;
	}
	else
	{
		host = std::string(url , 0 , slash2 - url );
		this->Path = slash2;
	}

	//ユーザ名とパスワード
	const char * user = strstr(host.c_str() , "@");
	if (user != NULL)
	{
		this->UserName = std::string(host.c_str() , user - host.c_str() );

		const char * password = strstr(user + 1 , ":");
		if (password != NULL)
		{
			this->UserName = std::string(user + 1, password - (user + 1) );
		}
		host = password + 1;
	}

	//ホスト名とポート番号
	const char * port = strstr(host.c_str() , ":");
	if (port != NULL)
	{
		this->Port = atoi( port + 1 );
		this->Host = std::string( host.c_str() , port - host.c_str() );
	}
	else
	{
		this->Host = host;
	}

	//ポートの自動認識
	if (this->Port == -1)
	{
		if (this->Protocol == "http")
		{
			this->Port = 80;
		}
		else if (this->Protocol == "https")
		{
			this->Port = 443;
		}
	}
}
/*
void XLUrlParser::test()
{
	{
		XLUrlParser a;
		std::string r;
		int rr;
		a.Parse("http://www.yahoo.co.jp/");
		assert( (r = a.getProtocol() ) == "http");
		assert( (r = a.getUserName() ) == "");
		assert( (r = a.getPassword() ) == "");
		assert( (rr = a.getPort() ) == 80);
		assert( (r = a.getHost() ) == "www.yahoo.co.jp");
		assert( (r = a.getPath() ) == "/");
	}
	{
		XLUrlParser a;
		std::string r;
		int rr;
		a.Parse("http://www.yahoo.co.jp:1192/");
		assert( (r = a.getProtocol() ) == "http");
		assert( (r = a.getUserName() ) == "");
		assert( (r = a.getPassword() ) == "");
		assert( (rr = a.getPort() ) == 1192);
		assert( (r = a.getHost() ) == "www.yahoo.co.jp");
		assert( (r = a.getPath() ) == "/");
	}
	{
		XLUrlParser a;
		std::string r;
		int rr;
		a.Parse("www.yahoo.co.jp:1192/");
		assert( (r = a.getProtocol() ) == "http");
		assert( (r = a.getUserName() ) == "");
		assert( (r = a.getPassword() ) == "");
		assert( (rr = a.getPort() ) == 1192);
		assert( (r = a.getHost() ) == "www.yahoo.co.jp");
		assert( (r = a.getPath() ) == "/");
	}
	{
		XLUrlParser a;
		std::string r;
		int rr;
		a.Parse("www.yahoo.co.jp/");
		assert( (r = a.getProtocol() ) == "http");
		assert( (r = a.getUserName() ) == "");
		assert( (r = a.getPassword() ) == "");
		assert( (rr = a.getPort() ) == 80);
		assert( (r = a.getHost() ) == "www.yahoo.co.jp");
		assert( (r = a.getPath() ) == "/");
	}
	{
		XLUrlParser a;
		std::string r;
		int rr;
		a.Parse("www.yahoo.co.jp/abcdf/ewfew/a.php");
		assert( (r = a.getProtocol() ) == "http");
		assert( (r = a.getUserName() ) == "");
		assert( (r = a.getPassword() ) == "");
		assert( (rr = a.getPort() ) == 80);
		assert( (r = a.getHost() ) == "www.yahoo.co.jp");
		assert( (r = a.getPath() ) == "/abcdf/ewfew/a.php");
	}
}
*/