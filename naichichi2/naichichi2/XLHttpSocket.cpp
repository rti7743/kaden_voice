#include "common.h"
#include "XLException.h"
#include "XLHttpSocket.h"

#include "XLUrlParser.h"
#include "XLHttpHeader.h"

std::string XLHttpSocket::Get(const std::string& url)
{
	const std::map<std::string,std::string> header;
	return Get(url,header,60);
}

std::string XLHttpSocket::Get(const std::string& url,unsigned int timeout)
{
	const std::map<std::string,std::string> header;
	return Get(url,header,timeout);
}

std::string XLHttpSocket::Get(const std::string& url,const std::map<std::string,std::string> & header)
{
	return Get(url,header,60);
}

std::string XLHttpSocket::Get(const std::string& url,const std::map<std::string,std::string> & header,unsigned int timeout)
{
	std::vector<char> retBinary;
	GetBinary(url,header,timeout,&retBinary);

	if (retBinary.empty()) return "";
	return std::string( &retBinary[0] , 0 , retBinary.size() );
}

void XLHttpSocket::GetBinary(const std::string& url,const std::map<std::string,std::string> & header,unsigned int timeout,std::vector<char>* retBinary)
{
	retBinary->clear();

	//URLパース
	XLUrlParser urlparse;
	urlparse.Parse(url);

	XLSocket socket;
	socket.CreateTCP();
	socket.Connect( urlparse.getHost() , urlparse.getPort() );

	//送信ヘッダーの準備
	std::string sendstring;
	sendstring = "GET " + urlparse.getPath() + " HTTP/1.0\r\n";
	for(auto headerit = header.begin() ; header.end() != headerit ; ++headerit)
	{
		sendstring += XLStringUtil::HeaderUpperCamel(headerit->first) + ": " + headerit->second + "\r\n";
	}
	if ( header.find("host") == header.end())
	{
		sendstring += "Host: " + urlparse.getHost() + "\r\n";
	}
	if ( header.find("connection") == header.end())
	{
		sendstring += "Connection: close\r\n";
	}
	sendstring += "\r\n";

	socket.Send(sendstring.c_str() ,sendstring.size() , 0 , timeout);
	
	//結果の受信
	return HTTPRecv( &socket , timeout,retBinary);
}


std::string XLHttpSocket::Post(const std::string& url,const char * postBinaryData,unsigned int postBinaryLength)
{
	const std::map<std::string,std::string> header;
	return Post(url,header,60,postBinaryData,postBinaryLength);
}

std::string XLHttpSocket::Post(const std::string& url,unsigned int timeout,const char * postBinaryData,unsigned int postBinaryLength)
{
	const std::map<std::string,std::string> header;
	return Post(url,header,timeout,postBinaryData,postBinaryLength);
}

std::string XLHttpSocket::Post(const std::string& url,const std::map<std::string,std::string> & header,const char * postBinaryData,unsigned int postBinaryLength)
{
	return Post(url,header,60,postBinaryData,postBinaryLength);
}

std::string XLHttpSocket::Post(const std::string& url,const std::map<std::string,std::string> & header,unsigned int timeout,const char * postBinaryData,unsigned int postBinaryLength)
{
	std::vector<char> retBinary;
	PostBinary(url,header,timeout,&retBinary,postBinaryData,postBinaryLength);

	if (retBinary.empty()) return "";
	return std::string( &retBinary[0] , 0 , retBinary.size() );
}

std::string XLHttpSocket::Post(const std::string& url,const std::map<std::string,std::string> & header,unsigned int timeout,const std::vector<char> postBinaryData)
{
	std::vector<char> retBinary;
	PostBinary(url,header,timeout,&retBinary,&postBinaryData[0],postBinaryData.size() );

	return &retBinary[0];
}

void XLHttpSocket::PostBinary(const std::string& url,const std::map<std::string,std::string> & header,unsigned int timeout,std::vector<char>* retBinary,const char * postBinaryData,unsigned int postBinaryLength)
{
	retBinary->clear();

	//URLパース
	XLUrlParser urlparse;
	urlparse.Parse(url);

	XLSocket socket;
	socket.CreateTCP();
	socket.Connect( urlparse.getHost() , urlparse.getPort() );

	//送信ヘッダーの準備
	std::string sendstring;
	sendstring = "POST " + urlparse.getPath() + " HTTP/1.0\r\n";
	for(auto headerit = header.begin() ; header.end() != headerit ; ++headerit)
	{
		sendstring += XLStringUtil::HeaderUpperCamel(headerit->first) + ": " + headerit->second + "\r\n";
	}
	if ( header.find("host") == header.end())
	{
		sendstring += "Host: " + urlparse.getHost() + "\r\n";
	}
	if ( header.find("connection") == header.end())
	{
		sendstring += "Connection: close\r\n";
	}
	if ( header.find("content-length") == header.end())
	{
		sendstring += "Content-Length: " + num2str(postBinaryLength) + "\r\n";
	}
	sendstring += "\r\n";

	socket.Send(sendstring.c_str() ,sendstring.size() , 0 , timeout);
	socket.Send(postBinaryData     ,postBinaryLength  , 0 , timeout);

	//結果の受信
	return HTTPRecv( &socket , timeout,retBinary);
}


void XLHttpSocket::PostBinaryCallback(const std::string& url,const std::map<std::string,std::string> & header,unsigned int timeout,std::vector<char>* retBinary,std::function<unsigned int (char* buf,unsigned int len) > callback)
{
	retBinary->clear();

	//URLパース
	XLUrlParser urlparse;
	urlparse.Parse(url);

	XLSocket socket;
	socket.CreateTCP();
	socket.Connect( urlparse.getHost() , urlparse.getPort() );

	//送信ヘッダーの準備
	std::string sendstring;
	sendstring = "POST " + urlparse.getPath() + " HTTP/1.0\r\n";
	for(auto headerit = header.begin() ; header.end() != headerit ; ++headerit)
	{
		sendstring += XLStringUtil::HeaderUpperCamel(headerit->first) + ": " + headerit->second + "\r\n";
	}
	if ( header.find("host") == header.end())
	{
		sendstring += "Host: " + urlparse.getHost() + "\r\n";
	}
	if ( header.find("connection") == header.end())
	{
		sendstring += "Connection: close\r\n";
	}
	if ( header.find("transfer-encoding") == header.end())
	{
		sendstring += "Transfer-Encoding: chunked\r\n";
	}
	sendstring += "\r\n";


	socket.Send(sendstring.c_str() ,sendstring.size() , 0 , timeout);
	
	//コールバックを利用してデータを詰め込む.
	std::vector<char> buffermalloc(65535 + 2);
	const unsigned int buffersize = buffermalloc.size();
	char * buffer = &buffermalloc[0];
	while(1)
	{
		unsigned int size = callback(buffer,buffersize );
		if (size == 0)
		{
			//終端マーク
			socket.Send("0\r\n",3 , 0 , timeout);
			break;
		}
		//何バイトデータが続くのかを書く.
		char sizebuffer[10];
		int sizelenth = sprintf(sizebuffer,"%x\r\n",size);
		sizebuffer[size + 0] = '\r';
		sizebuffer[size + 1] = '\n';
		socket.Send(sizebuffer,size + 2 , 0 , timeout);
		//本データを書く
		socket.Send(buffer,size , 0 , timeout);
	}

	//結果の受信
	return HTTPRecv( &socket , timeout,retBinary);
}

void XLHttpSocket::HTTPRecv(XLSocket * socket ,unsigned int timeout, std::vector<char>* retBinary)
{
	std::vector<char> buffermalloc(65535);
	const unsigned int buffersize = buffermalloc.size() - 1;
	char * buffer = &buffermalloc[0];

	//まずヘッダーのパース
	XLHttpHeader header;
	int size = socket->Recv(buffer,buffersize,0,timeout);
	if (size <= 0)
	{
		throw XLException("受信中にエラー");
	}
	buffer[size] = '\0';
	if (! header.Parse(buffer) )
	{
		throw XLException("ヘッダーパース中にエラーが発生しました");
	}
	//ヘッダーとボディを分離する
	retBinary->insert(retBinary->end() ,buffer + header.getHeaderSize() ,buffer + size + 1);

	//まだボディが続く場合は読み込む
	while(1)
	{
		size = socket->Recv(buffer,buffersize,0,timeout);
		if (size <= 0)
		{
			break;
		}
		retBinary->insert(retBinary->end() ,buffer,buffer +size );
	}
}
