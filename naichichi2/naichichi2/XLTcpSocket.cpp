// XLTcpSocket.cpp: XLTcpSocket クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "comm.h"
#include "XLTcpSocket.h"



//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

XLTcpSocket::XLTcpSocket()
{
	this->Connected = false;
}

XLTcpSocket::~XLTcpSocket()
{
	ShutDown();
	Close();
}

XLTcpSocket::XLTcpSocket(SOCKET inSocket) : XLSocket(inSocket)
{
	this->Connected = false;
}

void XLTcpSocket::Create()
{
	XLSocket::Create(SOCK_STREAM , 0);
}




void XLTcpSocket::Connect(const string &inHost , int inPort)	throw(XLIOException)
{
	ASSERT(this->Socket != INVALID_SOCKET);
	ASSERT(this->Connected == false);

	SOCKADDR_IN	sai = {0};
	
	//IP?
	unsigned long connectIP = inet_addr( inHost.c_str() );
	if(connectIP==INADDR_NONE)
	{
		//名前のようなので解決してみる.
		HOSTENT	* hostEnt;
		hostEnt=gethostbyname(inHost.c_str() );
		if(!hostEnt)
		{
			throw XLIOException(ETRACE + "ホスト名を解決できません" + ErrorToMesage(::WSAGetLastError() ) );
		}
		sai.sin_addr=*((IN_ADDR*)*hostEnt->h_addr_list);
	}
	else
	{
		sai.sin_addr.S_un.S_addr = connectIP;
	}

	sai.sin_port=htons((unsigned short)inPort);
	sai.sin_family=AF_INET;

	this->Connect(&sai) ;
}

void XLTcpSocket::Connect(const SOCKADDR_IN * inSai) throw(XLIOException)
{
	ASSERT(this->Socket != INVALID_SOCKET);
	ASSERT(this->Connected == false);

	//接続を試みる.
	const int ret = connect(Socket,(SOCKADDR*)inSai,sizeof(SOCKADDR_IN));
	if( ret == SOCKET_ERROR )
	{	
		throw XLIOException(ETRACE + "接続に失敗しました" + ErrorToMesage( ::WSAGetLastError() ) );
	}

	this->Connected = true;
}

void XLTcpSocket::Bind(unsigned long inBindIP , int inPort) throw(XLIOException)
{
	ASSERT(this->Socket != INVALID_SOCKET);
	ASSERT(this->Connected == false);

	int	Opt = 1;
	this->SetSockopt(SOL_SOCKET, SO_REUSEADDR, (const char*)&Opt , sizeof(int) );

	SOCKADDR_IN	sai = {0};
	sai.sin_family			= AF_INET;
	sai.sin_addr.s_addr	=   htonl(inBindIP);
	sai.sin_port			= htons((unsigned short)inPort);

	this->Bind( &sai );
}

void XLTcpSocket::Bind(const SOCKADDR_IN * inSai) throw(XLIOException)
{
	ASSERT(this->Socket != INVALID_SOCKET);
	ASSERT(this->Connected == false);

	int ret = ::bind( this->Socket , (struct sockaddr*) inSai , sizeof(sockaddr) );
	if (ret < 0)
	{
		throw XLIOException(ETRACE + "bind に失敗しました" + ErrorToMesage( ::WSAGetLastError() ) );
	}

	this->Connected  = true;
}

void XLTcpSocket::Listen(int inBacklog ) throw(XLIOException)
{
	int ret = ::listen( this->Socket , inBacklog );
	if (ret < 0)
	{
		throw XLIOException(ETRACE + "listen に失敗しました" + ErrorToMesage( ::WSAGetLastError() ) );
	}
}

XLTcpSocket* XLTcpSocket::Accept() throw(XLIOException)
{
	ASSERT(Socket != INVALID_SOCKET);

	SOCKET  newSock = ::accept( this->Socket , (struct sockaddr*) NULL , NULL);
	if (newSock <= 0 )
	{
		throw XLIOException(ETRACE + "accept に失敗しました" + ErrorToMesage( ::WSAGetLastError() ) );
	}
	//新規に作成して返す
	return new XLTcpSocket(newSock);
}

void XLTcpSocket::ShutDown()
{
	if ( this->Socket == INVALID_SOCKET) return ;
	if ( this->Connected == false)	return ;

	::shutdown( this->Socket , 1 );

	//この時間になってもつないでいる場合は、即切る.
	DWORD TimeOutTime = ::time(NULL) + 10; //10秒待つ.

	//保留中のすべてのデータを受け取る.
	int		ret;
	char	buffer[1024];
	while(1)
	{
		ret = ::recv( this->Socket , buffer , 1024 , 0 );
		
		if ( ret <= 0 || ret == SOCKET_ERROR)	break;

		//タイムアウトチェック
		if ( TimeOutTime < ::time(NULL) ) 
		{
			break;
		}
	}
	//データをこれ以上受信しないことを相手に伝える.
	::shutdown( this->Socket , 2);

	this->Connected = false;
}

int XLTcpSocket::Send( const char* inBuffer ,int inBufferLen , int inFlags , int inTimeOutS) throw(XLIOException)
{
	Select(SELECT_TYPE_WRITE , inTimeOutS);

	int ret = ::send( this->Socket , inBuffer , inBufferLen , inFlags);
	if (ret < 0)
	{
		int errorCode = ::WSAGetLastError();
		if (errorCode == WSAEWOULDBLOCK)
		{
			::WSASetLastError(errorCode);
			return ret;
		}
		throw XLIOException(ETRACE + "send に失敗しました" + ErrorToMesage( errorCode ) );
	}
	return ret;
}

int XLTcpSocket::Recv( char* outBuffer ,int inBufferLen , int inFlags , int inTimeOutS) throw(XLIOException)
{
	Select(SELECT_TYPE_READ , inTimeOutS);

	int ret = ::recv( this->Socket , outBuffer , inBufferLen , inFlags);
	if (ret < 0)
	{
		int errorCode = ::WSAGetLastError();
		if (errorCode == WSAEWOULDBLOCK)
		{
			::WSASetLastError(errorCode);
			return ret;
		}
		throw XLIOException(ETRACE + "recv に失敗しました" + ErrorToMesage( errorCode ) );
	}
	return ret;
}


void XLTcpSocket::Select(SELECT_TYPE inType , long inTimeOutS) throw(XLIOException)
{
	timeval useTimeVal;
	FD_SET	flg;
	//セレクトの戻り値
	int i;
	//短い時間でチェックします.
	for(i = 0 ; i < inTimeOutS ; i++)
	{
		//秒.
		useTimeVal.tv_sec  = 1;
		//秒ms
		useTimeVal.tv_usec = 0;
		FD_ZERO(&flg);
		FD_SET(Socket , &flg);

		int	ret = 0;
		switch(inType)
		{
		case SELECT_TYPE_READ:
			ret = select(2 , &flg, NULL , NULL , &useTimeVal);
			break;
		case SELECT_TYPE_WRITE:
			ret = select(2 , NULL, &flg , NULL , &useTimeVal);
			break;
		}
		//状態が変化した.
		if (ret != 0) break;	
	}
	//タイムアウトです.
	if (i == inTimeOutS)
	{
		throw XLIOException(ETRACE + "タイムアウトが発生しました");
	}

	if ( ! FD_ISSET( this->Socket , &flg ) )
	{
		throw XLIOException(ETRACE + "select 中にエラーが発生しました" + ErrorToMesage( ::WSAGetLastError() ) );
	}

}