#include "common.h"
#include "XLHttpRequerst.h"
#include "XLUrlParser.h"
#include "XLStringUtil.h"
#include <vector>
//#include <boost/asio.hpp>
//#include "xreturn.h"

xreturn::r<bool> XLHttpRequerst::Download(const std::string & url , const std::map<std::string,std::string> & header ,const std::map<std::string,std::string> & option)
{
	this->OutData.clear();

	//URLパース
	XLUrlParser urlparse;
	urlparse.Parse(url);

	//ASIO作成
	boost::asio::io_service io_service;

	 //名前解決
	boost::asio::ip::tcp::resolver resolver(io_service);
	boost::asio::ip::tcp::resolver::query query(urlparse.getHost(), "http");
	boost::asio::ip::tcp::endpoint endpoint(*resolver.resolve(query));

    boost::asio::ip::tcp::socket socket(io_service);

	//接続
	boost::system::error_code error = boost::asio::error::host_not_found;
	socket.connect(endpoint,error);
    if (error)
	{
		return xreturn::error("ホストに接続できません URL:" + url);
	}

	//送信
    boost::asio::streambuf request;
    std::ostream request_stream(&request);
	bool ispost = ( option.find("method") != option.end() && option.find("method")->second == "post" );
	if (ispost)
	{
		if (option.find("data") == option.end())
		{
			return xreturn::error("POSTなのにデータの内容がないよう。sample: option[\"data\"] = \"a=123&b=1234&sample=sakura\"; ");
		}
		request_stream << "POST " << urlparse.getPath() << " HTTP/1.0\r\n";
	}
	else
	{
		request_stream << "GET " << urlparse.getPath() << " HTTP/1.0\r\n";
	}

	//ユーザのヘッダを送信
	for(auto headerit = header.begin() ; header.end() != headerit ; ++headerit)
	{
		request_stream << XLStringUtil::HeaderUpperCamel(headerit->first) + ": " << headerit->second << "\r\n";
	}

	//ユーザがHostや Connectionなどを指定しなかったらつけてあげる.
	if ( header.find("host") == header.end() )
	{
		request_stream << "Host: " << urlparse.getHost() << "\r\n";
	}
	if ( header.find("connection") == header.end())
	{
		request_stream << "Connection: close\r\n";
	}
	if ( header.find("accept") == header.end())
	{
		request_stream << "Accept: */*\r\n";
	}
	//date とかどうしよう

	//POSTで Content-Length が未指定の場合はつけてあげる.
	if (ispost && header.find("content-length") == header.end())
	{
		assert(option.find("data") != option.end());
		request_stream << "Content-Length: " << option.find("data")->second.size() << "\r\n";
	}

	//ヘッダー終端
	request_stream << "\r\n";

	//POSTの場合データ送信
	if (ispost)
	{
		request_stream << option.find("data")->second; // 改行は不要.
	}
	request_stream << std::flush; 

    // Send the request.
    boost::asio::write(socket, request);

    // Read the response status line.
    boost::asio::streambuf response_header;
    // Read the response headers, which are terminated by a blank line.
    boost::asio::read_until(socket, response_header, "\r\n\r\n");

    // Read until EOF, writing data to output as we go.
    boost::asio::streambuf response_body;
    while (boost::asio::read(socket, response_body, boost::asio::transfer_at_least(1), error))
	{
	}
	//まとめて読み出す.
	const char* data = boost::asio::buffer_cast<const char*>(response_body.data());
	int size = response_body.size();

	this->OutData.insert(this->OutData.end() ,data , data + size );

	return true;
}

xreturn::r<bool> XLHttpRequerst::saveFile(const std::string & filename) const
{
	FILE * fp = fopen(filename.c_str() , "wb");
	if (!fp)
	{
		int code = errno;
		return xreturn::errnoError(std::string() + "ファイル:" + filename + "に書き込めません",code);
	}
	fwrite( &this->OutData[0] , 1 , this->OutData.size() , fp );
	fclose(fp);

	return true;
}

