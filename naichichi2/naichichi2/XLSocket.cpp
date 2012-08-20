// XLSocket.cpp: XLSocket クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////
#if _MSC_VER
	#pragma comment(lib, "ws2_32.lib")
#endif
#include <string>
#include <assert.h>
#include <time.h>
#include "XLException.h"
#include "XLSocket.h"

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

XLSocket::XLSocket()
{
	this->Socket = INVALID_SOCKET;
	this->Connected = false;
}

XLSocket::~XLSocket()
{
	if (this->Connected)
	{
		Shutdown();
	}
	if (this->Socket != INVALID_SOCKET)
	{
		Close();
	}
}

XLSocket::XLSocket(SOCKET inSocket)
{
	this->Socket = inSocket;
}


void XLSocket::CreateLow( int type , int protocol )
{
	Close();

	//ソケットを開く
	this->Socket = ::socket(AF_INET,type,protocol);
	if (this->Socket == INVALID_SOCKET)
	{
		throw XLException( ErrorToMesage( ::WSAGetLastError() ) );
	}
}
void XLSocket::CreateTCP( )
{
	CreateLow(SOCK_STREAM , 0);
}
void XLSocket::CreateUDP(  )
{
	CreateLow(SOCK_STREAM , 0);
}


void XLSocket::Close()
{
	if (this->Socket == INVALID_SOCKET)
	{
		return ;
	}

	::closesocket(this->Socket);
	this->Socket = INVALID_SOCKET;
}

int XLSocket::IoctlSocket(long inCmd , unsigned long * ioArgp)
{
	assert(this->Socket != INVALID_SOCKET);

	int ret =  ::ioctlsocket(this->Socket , inCmd , ioArgp);
	if (ret == SOCKET_ERROR )
	{
		throw XLException( ErrorToMesage( ::WSAGetLastError() ) );
	}
	return ret;
}


int XLSocket::SetSockopt( int level, int optname,  const char * optval,  int optlen )
{
	assert(this->Socket != INVALID_SOCKET);

	return ::setsockopt(this->Socket , level , optname , optval , optlen);
}

int XLSocket::GetSockopt( int level, int optname,  char * optval,  int * optlen )
{
	assert(this->Socket != INVALID_SOCKET);
	
	return ::getsockopt(this->Socket , level , optname , optval , optlen);
}

std::string XLSocket::ErrorToMesage(DWORD inErrorCode)
{
#if _MSC_VER
	switch(inErrorCode)
	{
	case WSAEINTR: return std::string(" 関数呼び出しが中断された。\nWinSock 1.1 のブロッキング関数呼び出しが、WSACancelBlockingCall() によって中断された。");
	case WSAEBADF: return std::string("無効なファイルハンドル。\nソケット関数に渡されたファイルハンドル（ソケットハンドル）が不正である。（WSAENOTSOCKと同義で用いられる）");
	case WSAEACCES: return std::string("アクセスが拒否された。\nconnect()：ブロードキャストアドレスで接続しようとしたソケットの SO_BROADCASTオプション が有効になっていない。\nsendto()：ブロードキャストアドレスに送信しようとしたソケットの SO_BROADCASTオプションが有効になっていない。\naccept()：受付しようとした接続要求がすでにタイムアウトになっている、もしくは取り消されている。");
	case WSAEFAULT: return std::string("無効なバッファアドレス\n関数に渡されたバッファを指すポインタが無効なアドレス空間を指している。またはバッファのサイズが小さすぎる。");
	case WSAEINVAL: return std::string("無効な引数が渡された。\nlisten()を呼び出さずにaccept()を呼び出した。\nバインド済みのソケットに対してbind()しようとした。\nlisten()で待機中のソケットでconnect()しようとした。\n関数に渡されたパラメータに無効な値が含まれている。");
	case WSAEMFILE: return std::string("使用中のソケットの数が多すぎる。\n同時に利用できるソケット数の最大に達しており、新たにソケットを作成できない。");
//	case WSAEWOULDBLOCK: return std::string("操作はブロッキングされる。\nノンブロッキングモードでソケット関数が呼び出されため、関数は即座にリターンしたが要求された操作は完了していない。");
	case WSAEINPROGRESS: return std::string("すでにブロッキング手続きが実行されている。\nブロッキング関数が実行中であるか、サービスプロバイダがコールバック関数を処理中である。WinSockでは１つのプロセスで同時に複数のブロッキング操作をすることはできないため、前回呼び出した操作が完全に終了するまで次の操作を開始することはできない。");
	case WSAEALREADY: return std::string("要求された操作は既に実行中、または実行済み。\nキャンセルしようとした非同期操作が既にキャンセルされている。\nconnect()を呼び出したが、既に前回の呼び出しによって接続処理中である。");
	case WSAENOTSOCK: return std::string("指定されたソケットが無効である。\nソケットハンドルとして無効な値がソケット関数に渡された。");
	case WSAEMSGSIZE: return std::string("メッセージサイズが大きすぎる。\n送信、または受信しようとしたメッセージが、基盤となるプロトコルのサポートしている最大メッセージサイズを超えている。ioctlsocket()でバッファサイズを増やすことで回避可能なこともある。");
	case WSAEPROTOTYPE: return std::string("ソケットは要求されたプロトコルに適合していない。\nsocket()を呼び出すときに指定したプロトコルタイプがソケットタイプと適合していない。メッセージ型プロトコルでストリームソケットを作成しようとした場合など。");
	case WSAENOPROTOOPT: return std::string("不正なプロトコルオプション。\n指定したソケットオプションまたはioctlsocket()での操作が、そのプロトコルではサポートされてない。\nストリームソケットに SO_BROADCAST を指定しようとした。");
	case WSAEPROTONOSUPPORT: return std::string("プロトコルがサポートされていない。\nサポートされていないプロトコルを指定した、またはプロトコルのサポートしていない操作を実行しようとした。");
	case WSAESOCKTNOSUPPORT: return std::string("指定されたソケットタイプはサポートされていない。");
	case WSAEOPNOTSUPP: return std::string("要求された操作はサポートされていない。\nバンド外データを送信/受信しようとしたソケットでそれがサポートされていない。\n非接続型(UDPなど)ソケットでaccept()を呼び出した。");
	case WSAEPFNOSUPPORT: return std::string("プロトコルファミリがサポートされていない。");
	case WSAEAFNOSUPPORT: return std::string("アドレスファミリがサポートされていない。");
	case WSAEADDRINUSE: return std::string("アドレスは既に使用中である。\nbind()しようとしたアドレスは、既にほかのソケットで使われている。同じローカルアドレスを複数のソケットで使うためには、SO_REUSEADDRソケットオプションをTRUEにする必要がある。");
	case WSAEADDRNOTAVAIL: return std::string("無効なネットワークアドレス。\nソケット関数に渡されたネットワークアドレスに無効な部分がある。");
	case WSAENETDOWN: return std::string("ネットワークがダウンしている。\nネットワークシステムが何らかの障害を起こし、機能停止しているため要求された操作が実行できない。");
	case WSAENETUNREACH: return std::string("指定されたネットワークホストに到達できない。\nWSAEHOSTUNREACHと同義で使われている。");
	case WSAENETRESET: return std::string("ネットワーク接続が破棄された。\nキープアライブを行っている接続で、何らかの障害のためタイムアウトが検出され接続が閉じられた。");
	case WSAECONNABORTED: return std::string("ネットワーク接続が破棄された。");
	case WSAECONNRESET: return std::string("ネットワーク接続が相手によって破棄された。");
	case WSAENOBUFS: return std::string("バッファが不足している。\nメモリ不足のためWinSockが操作に必要なバッファを確保することができない。または、バッファを必要とする関数呼び出しで、小さすぎるバッファが渡された。");
	case WSAEISCONN: return std::string("ソケットは既に接続されている。\n接続中のソケットに対してconnect()、listen()、bind()等の操作を行おうとした。\n接続中のソケットでsendto()を実行しようとした。");
	case WSAENOTCONN: return std::string("ソケットは接続されていない。\n接続されていないソケットでsend()、getpeername()等の接続されたソケットを必要とする操作を実行しようとした。");
	case WSAESHUTDOWN: return std::string("ソケットはシャットダウンされている。\n既にシャットダウンされて送信/受信が停止されているソケットで、送信または受信の操作を実行しようとした。");
	case WSAETOOMANYREFS: return std::string("参照の数が多すぎる。");
	case WSAETIMEDOUT: return std::string("接続要求がタイムアウトした。\nconnect()を呼び出して接続を試みたが、（相手の応答がない等で）処理がタイムアウトになり接続要求がキャンセルされた。");
	case WSAECONNREFUSED: return std::string("接続が拒否された。");
	case WSAELOOP: return std::string("ループ。");
	case WSAENAMETOOLONG: return std::string("名前が長すぎる。");
	case WSAEHOSTDOWN: return std::string("ホストがダウンしている。\nWSAETIMEDOUTと同義。");
	case WSAEHOSTUNREACH: return std::string("ホストへの経路がない。\nネットワークの構造上到達できないホストに対して操作を実行しようとした。またはアドレスが不正である。");
	case WSAENOTEMPTY: return std::string("ディレクトリが空ではない。");
	case WSAEPROCLIM: return std::string("プロセスの数が多すぎる。\nWSAStartup()：既にWinSockが管理できる最大プロセス数に達しており処理が実行できない。");
	case WSAEUSERS: return std::string("ユーザーの数が多すぎる。");
	case WSAEDQUOT: return std::string("ディスククォータ。");
	case WSAESTALE: return std::string("実行しようとした操作は廃止されている。");
	case WSAEREMOTE: return std::string("リモート。");
	case WSASYSNOTREADY: return std::string("ネットワークサブシステムが利用できない。\nWSAStartup()：ネットワークサブシステムが利用できない、または正しくセットアップされていないため機能していない。");
	case WSAVERNOTSUPPORTED: return std::string("Winsock.dllのバージョンが範囲外である。\nWSAStartup()：要求したWinSockバージョンは、現在の実装ではサポートされていない。");
	case WSANOTINITIALISED: return std::string("WinSockシステムが初期化されていない。\nWinSock関数を実行しようとしたが、WSAStartup()が正常に実行されていないため機能しない。");
	case WSAEDISCON: return std::string("シャットダウン処理中。\n接続が相手の処理によって切断されようとしている。");
	case WSAHOST_NOT_FOUND: return std::string("ホストが見つからない。\nDNSなどの名前解決サービスが指定されたホストを見つけられなかった。プロトコルやサービスのクエリー操作においても返される。");
	case WSATRY_AGAIN: return std::string("指定されたホストが見つからない、またはサービスの異常。");
	case WSANO_RECOVERY: return std::string("回復不能なエラーが発生した。\nデータベース検索の場合は、名前解決サービスが使用できないことを意味する。");
	case WSANO_DATA: return std::string("要求されたタイプのデータレコードが見つからない。");
	}
#endif
	return "";
}

void XLSocket::Connect(const std::string &inHost , int inPort)
{
	assert(this->Socket != INVALID_SOCKET);
	assert(this->Connected == false);

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
			throw XLException("ホスト名を解決できません" + ErrorToMesage(::WSAGetLastError() ) );
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

void XLSocket::Connect(const SOCKADDR_IN * inSai) 
{
	assert(this->Socket != INVALID_SOCKET);
	assert(this->Connected == false);

	//接続を試みる.
	const int ret = connect(Socket,(SOCKADDR*)inSai,sizeof(SOCKADDR_IN));
	if( ret == SOCKET_ERROR )
	{	
		throw XLException("接続に失敗しました" + ErrorToMesage( ::WSAGetLastError() ) );
	}

	this->Connected = true;
}

void XLSocket::Bind(unsigned long inBindIP , int inPort)
{
	assert(this->Socket != INVALID_SOCKET);
	assert(this->Connected == false);

	int	Opt = 1;
	this->SetSockopt(SOL_SOCKET, SO_REUSEADDR, (const char*)&Opt , sizeof(int) );

	SOCKADDR_IN	sai = {0};
	sai.sin_family			= AF_INET;
	sai.sin_addr.s_addr	=   htonl(inBindIP);
	sai.sin_port			= htons((unsigned short)inPort);

	this->Bind( &sai );
}

void XLSocket::Bind(const SOCKADDR_IN * inSai)
{
	assert(this->Socket != INVALID_SOCKET);
	assert(this->Connected == false);

	int ret = ::bind( this->Socket , (struct sockaddr*) inSai , sizeof(sockaddr) );
	if (ret < 0)
	{
		throw XLException("bind に失敗しました" + ErrorToMesage( ::WSAGetLastError() ) );
	}

	this->Connected  = true;
}

void XLSocket::Listen(int inBacklog )
{
	int ret = ::listen( this->Socket , inBacklog );
	if (ret < 0)
	{
		throw XLException("listen に失敗しました" + ErrorToMesage( ::WSAGetLastError() ) );
	}
}

XLSocket* XLSocket::Accept()
{
	assert(Socket != INVALID_SOCKET);

	SOCKET  newSock = ::accept( this->Socket , (struct sockaddr*) NULL , NULL);
	if (newSock <= 0 )
	{
		throw XLException("accept に失敗しました" + ErrorToMesage( ::WSAGetLastError() ) );
	}
	//新規に作成して返す
	return new XLSocket(newSock);
}

void XLSocket::Shutdown()
{
	if ( this->Socket == INVALID_SOCKET) return ;
	if ( this->Connected == false)	return ;

	::shutdown( this->Socket , 1 );

	//この時間になってもつないでいる場合は、即切る.
	time_t TimeOutTime = time(NULL) + 10; //10秒待つ.

	//保留中のすべてのデータを受け取る.
	int		ret;
	char	buffer[1024];
	while(1)
	{
		ret = ::recv( this->Socket , buffer , 1024 , 0 );
		
		if ( ret <= 0 || ret == SOCKET_ERROR)	break;

		//タイムアウトチェック
		if ( TimeOutTime < time(NULL) ) 
		{
			break;
		}
	}
	//データをこれ以上受信しないことを相手に伝える.
	::shutdown( this->Socket , 2);

	this->Connected = false;
}

int XLSocket::Send( const char* inBuffer ,int inBufferLen , int inFlags , int inTimeOutS)
{
	Select(SELECT_TYPE_WRITE , inTimeOutS);

	int ret = ::send( this->Socket , inBuffer , inBufferLen , inFlags);
	if (ret < 0)
	{
		int errorCode = ::WSAGetLastError();
//		if (errorCode == WSAEWOULDBLOCK)
//		{
//			::WSASetLastError(errorCode);
//			return ret;
//		}
		throw XLException("send に失敗しました" + ErrorToMesage( errorCode ) );
	}
	return ret;
}

int XLSocket::Recv( char* outBuffer ,int inBufferLen , int inFlags , int inTimeOutS)
{
	Select(SELECT_TYPE_READ , inTimeOutS);

	int ret = ::recv( this->Socket , outBuffer , inBufferLen , inFlags);
	if (ret < 0)
	{
		return -1;
	}
	return ret;
}


void XLSocket::Select(SELECT_TYPE inType , long inTimeOutS)
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
		throw XLException("タイムアウトが発生しました");
	}

	if ( ! FD_ISSET( this->Socket , &flg ) )
	{
		throw XLException("select 中にエラーが発生しました" + ErrorToMesage( ::WSAGetLastError() ) );
	}
}
