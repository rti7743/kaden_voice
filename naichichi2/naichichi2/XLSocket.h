// XLSocket.h: XLSocket クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XLSOCKET_H__137F6EB5_32A1_46CD_9CA7_EC6E9C6A6E6A__INCLUDED_)
#define AFX_XLSOCKET_H__137F6EB5_32A1_46CD_9CA7_EC6E9C6A6E6A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if _MSC_VER
	#include <Winsock2.h>
	#include <mswsock.h>
#endif


class XLSocket  
{
public:
	/**
	 * XLSocket:	ソケット作成
	 *
	 * @return  
	 */
	XLSocket();
	virtual ~XLSocket();

	/**
	 * XLSocket:		作成済みソケットの割り当て.
	 *
	 * @param inSocket 
	 * @return  
	 */
	XLSocket(SOCKET inSocket);

	/**
	 * Create:	ソケットの作成
	 *
	 * @param type	ソケットの種類	SOCK_STREAM : TCP
	 *								SOCK_DGRAM : UDP
	 * @return void 
	 */
	void CreateLow( int type , int protocol );
	void CreateTCP( );
	void CreateUDP( );
	
	/**
	 * Close:	ソケットを閉じる
	 *
	 * @return void 
	 */
	void Close();

	/**
	 * ioctlsocket:
	 *
	 * @param inCmd 
	 * @param ioArgp 
	 * @return  int	
	 */
	int IoctlSocket(long inCmd , unsigned long * ioArgp);

	int SetSockopt( int level, int optname,  const char * optval,  int optlen );

	int GetSockopt( int level, int optname,  char * optval,  int * optlen );

	void Connect(const std::string &inHost , int inPort);

	void Connect(const SOCKADDR_IN * inSai);

	void Bind(unsigned long inBindIP , int inPort);

	void Bind(const SOCKADDR_IN * inSai);

	void Listen(int inBacklog = SOMAXCONN);

	XLSocket* Accept();

	void Shutdown();

	int Send( const char* inBuffer ,int inBufferLen , int inFlags = 0 , int inTimeOutS = 60);

	int Recv( char* outBuffer ,int inBufferLen , int inFlags = 0 , int inTimeOutS = 60);

	enum SELECT_TYPE
	{
		 SELECT_TYPE_READ
		,SELECT_TYPE_WRITE
	};
	void Select(SELECT_TYPE inType , long inTimeOutS);
protected:

	/**
	 * ErrorToMesage:		ソケットエラーを文字列化
	 *
	 * @param inErrorCode	
	 * @return string 
	 */
	std::string ErrorToMesage(DWORD inErrorCode);

	/**
	 * Socket:	ソケットハンドル
	 */
	SOCKET	Socket;
	
	
	/**
	 * Connected:	接続しているかどうか？
	 */
	bool Connected;
};

#endif // !defined(AFX_XLSOCKET_H__137F6EB5_32A1_46CD_9CA7_EC6E9C6A6E6A__INCLUDED_)
