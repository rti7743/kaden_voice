// XLTcpSocket.h: XLTcpSocket クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XLTCPSOCKET_H__5F737517_0886_4D02_ABA0_3DD488B5FAE2__INCLUDED_)
#define AFX_XLTCPSOCKET_H__5F737517_0886_4D02_ABA0_3DD488B5FAE2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XLSocket.h"
#include "XLIOException.h"

class XLTcpSocket : public XLSocket  
{
public:
	enum SELECT_TYPE
	{
		SELECT_TYPE_READ ,
		SELECT_TYPE_WRITE ,
	};

public:
	XLTcpSocket();
	virtual ~XLTcpSocket();

	XLTcpSocket::XLTcpSocket(SOCKET inSocket) ;

	void Create();

	void Connect(const string &inHost , int inPort)	throw(XLIOException);

	void Connect(const SOCKADDR_IN * inSai) throw(XLIOException);

	void Bind(unsigned long inBindIP , int inPort) throw(XLIOException);

	void Bind(const SOCKADDR_IN * inSai) throw(XLIOException);

	void Listen(int inBacklog = SOMAXCONN) throw(XLIOException);

	XLTcpSocket* Accept() throw(XLIOException);

	void ShutDown();

	int Send( const char* inBuffer ,int inBufferLen , int inFlags = 0 , int inTimeOutS = 60) throw(XLIOException);

	int Recv( char* outBuffer ,int inBufferLen , int inFlags = 0 , int inTimeOutS = 60) throw(XLIOException);

	void Select(SELECT_TYPE inType , long inTimeOutS) throw(XLIOException);
private:
	//接続されていますか?
	bool	Connected;
};

#endif // !defined(AFX_XLTCPSOCKET_H__5F737517_0886_4D02_ABA0_3DD488B5FAE2__INCLUDED_)
