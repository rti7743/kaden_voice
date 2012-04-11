// XLSocket.cpp: XLSocket クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////


#include "common.h"
#include "XLSocket.h"

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

XLSocket::XLSocket()
{
	this->Socket = INVALID_SOCKET;
}

XLSocket::~XLSocket()
{

}

XLSocket::XLSocket(SOCKET inSocket)
{
	this->Socket = inSocket;
}


void XLSocket::Create( int type , int protocol ) throw (XLResourceException)
{
	Close();

	//ソケットを開く
	this->Socket = ::socket(AF_INET,type,protocol);
	if (this->Socket == INVALID_SOCKET)
	{
		throw XLException( ErrorToMesage( ::WSAGetLastError() ) );
	}
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

int XLSocket::IoctlSocket(long inCmd , unsigned long * ioArgp) throw(XLParamException)
{
	ASSERT(this->Socket != INVALID_SOCKET);

	int ret =  ::ioctlsocket(this->Socket , inCmd , ioArgp);
	if (ret == SOCKET_ERROR )
	{
		throw XLParamException( ErrorToMesage( ::WSAGetLastError() ) );
	}
	return ret;
}


int XLSocket::SetSockopt( int level, int optname,  const char * optval,  int optlen )
{
	ASSERT(this->Socket != INVALID_SOCKET);

	return ::setsockopt(this->Socket , level , optname , optval , optlen);
}

int XLSocket::GetSockopt( int level, int optname,  char * optval,  int * optlen )
{
	ASSERT(this->Socket != INVALID_SOCKET);
	
	return ::getsockopt(this->Socket , level , optname , optval , optlen);
}

string XLSocket::ErrorToMesage(DWORD inErrorCode)
{
	switch(inErrorCode)
	{
	case WSAEINTR: return string(" 関数呼び出しが中断された。\nWinSock 1.1 のブロッキング関数呼び出しが、WSACancelBlockingCall() によって中断された。");
	case WSAEBADF: return string("無効なファイルハンドル。\nソケット関数に渡されたファイルハンドル（ソケットハンドル）が不正である。（WSAENOTSOCKと同義で用いられる）");
	case WSAEACCES: return string("アクセスが拒否された。\nconnect()：ブロードキャストアドレスで接続しようとしたソケットの SO_BROADCASTオプション が有効になっていない。\nsendto()：ブロードキャストアドレスに送信しようとしたソケットの SO_BROADCASTオプションが有効になっていない。\naccept()：受付しようとした接続要求がすでにタイムアウトになっている、もしくは取り消されている。");
	case WSAEFAULT: return string("無効なバッファアドレス\n関数に渡されたバッファを指すポインタが無効なアドレス空間を指している。またはバッファのサイズが小さすぎる。");
	case WSAEINVAL: return string("無効な引数が渡された。\nlisten()を呼び出さずにaccept()を呼び出した。\nバインド済みのソケットに対してbind()しようとした。\nlisten()で待機中のソケットでconnect()しようとした。\n関数に渡されたパラメータに無効な値が含まれている。");
	case WSAEMFILE: return string("使用中のソケットの数が多すぎる。\n同時に利用できるソケット数の最大に達しており、新たにソケットを作成できない。");
//	case WSAEWOULDBLOCK: return string("操作はブロッキングされる。\nノンブロッキングモードでソケット関数が呼び出されため、関数は即座にリターンしたが要求された操作は完了していない。");
	case WSAEINPROGRESS: return string("すでにブロッキング手続きが実行されている。\nブロッキング関数が実行中であるか、サービスプロバイダがコールバック関数を処理中である。WinSockでは１つのプロセスで同時に複数のブロッキング操作をすることはできないため、前回呼び出した操作が完全に終了するまで次の操作を開始することはできない。");
	case WSAEALREADY: return string("要求された操作は既に実行中、または実行済み。\nキャンセルしようとした非同期操作が既にキャンセルされている。\nconnect()を呼び出したが、既に前回の呼び出しによって接続処理中である。");
	case WSAENOTSOCK: return string("指定されたソケットが無効である。\nソケットハンドルとして無効な値がソケット関数に渡された。");
	case WSAEMSGSIZE: return string("メッセージサイズが大きすぎる。\n送信、または受信しようとしたメッセージが、基盤となるプロトコルのサポートしている最大メッセージサイズを超えている。ioctlsocket()でバッファサイズを増やすことで回避可能なこともある。");
	case WSAEPROTOTYPE: return string("ソケットは要求されたプロトコルに適合していない。\nsocket()を呼び出すときに指定したプロトコルタイプがソケットタイプと適合していない。メッセージ型プロトコルでストリームソケットを作成しようとした場合など。");
	case WSAENOPROTOOPT: return string("不正なプロトコルオプション。\n指定したソケットオプションまたはioctlsocket()での操作が、そのプロトコルではサポートされてない。\nストリームソケットに SO_BROADCAST を指定しようとした。");
	case WSAEPROTONOSUPPORT: return string("プロトコルがサポートされていない。\nサポートされていないプロトコルを指定した、またはプロトコルのサポートしていない操作を実行しようとした。");
	case WSAESOCKTNOSUPPORT: return string("指定されたソケットタイプはサポートされていない。");
	case WSAEOPNOTSUPP: return string("要求された操作はサポートされていない。\nバンド外データを送信/受信しようとしたソケットでそれがサポートされていない。\n非接続型(UDPなど)ソケットでaccept()を呼び出した。");
	case WSAEPFNOSUPPORT: return string("プロトコルファミリがサポートされていない。");
	case WSAEAFNOSUPPORT: return string("アドレスファミリがサポートされていない。");
	case WSAEADDRINUSE: return string("アドレスは既に使用中である。\nbind()しようとしたアドレスは、既にほかのソケットで使われている。同じローカルアドレスを複数のソケットで使うためには、SO_REUSEADDRソケットオプションをTRUEにする必要がある。");
	case WSAEADDRNOTAVAIL: return string("無効なネットワークアドレス。\nソケット関数に渡されたネットワークアドレスに無効な部分がある。");
	case WSAENETDOWN: return string("ネットワークがダウンしている。\nネットワークシステムが何らかの障害を起こし、機能停止しているため要求された操作が実行できない。");
	case WSAENETUNREACH: return string("指定されたネットワークホストに到達できない。\nWSAEHOSTUNREACHと同義で使われている。");
	case WSAENETRESET: return string("ネットワーク接続が破棄された。\nキープアライブを行っている接続で、何らかの障害のためタイムアウトが検出され接続が閉じられた。");
	case WSAECONNABORTED: return string("ネットワーク接続が破棄された。");
	case WSAECONNRESET: return string("ネットワーク接続が相手によって破棄された。");
	case WSAENOBUFS: return string("バッファが不足している。\nメモリ不足のためWinSockが操作に必要なバッファを確保することができない。または、バッファを必要とする関数呼び出しで、小さすぎるバッファが渡された。");
	case WSAEISCONN: return string("ソケットは既に接続されている。\n接続中のソケットに対してconnect()、listen()、bind()等の操作を行おうとした。\n接続中のソケットでsendto()を実行しようとした。");
	case WSAENOTCONN: return string("ソケットは接続されていない。\n接続されていないソケットでsend()、getpeername()等の接続されたソケットを必要とする操作を実行しようとした。");
	case WSAESHUTDOWN: return string("ソケットはシャットダウンされている。\n既にシャットダウンされて送信/受信が停止されているソケットで、送信または受信の操作を実行しようとした。");
	case WSAETOOMANYREFS: return string("参照の数が多すぎる。");
	case WSAETIMEDOUT: return string("接続要求がタイムアウトした。\nconnect()を呼び出して接続を試みたが、（相手の応答がない等で）処理がタイムアウトになり接続要求がキャンセルされた。");
	case WSAECONNREFUSED: return string("接続が拒否された。");
	case WSAELOOP: return string("ループ。");
	case WSAENAMETOOLONG: return string("名前が長すぎる。");
	case WSAEHOSTDOWN: return string("ホストがダウンしている。\nWSAETIMEDOUTと同義。");
	case WSAEHOSTUNREACH: return string("ホストへの経路がない。\nネットワークの構造上到達できないホストに対して操作を実行しようとした。またはアドレスが不正である。");
	case WSAENOTEMPTY: return string("ディレクトリが空ではない。");
	case WSAEPROCLIM: return string("プロセスの数が多すぎる。\nWSAStartup()：既にWinSockが管理できる最大プロセス数に達しており処理が実行できない。");
	case WSAEUSERS: return string("ユーザーの数が多すぎる。");
	case WSAEDQUOT: return string("ディスククォータ。");
	case WSAESTALE: return string("実行しようとした操作は廃止されている。");
	case WSAEREMOTE: return string("リモート。");
	case WSASYSNOTREADY: return string("ネットワークサブシステムが利用できない。\nWSAStartup()：ネットワークサブシステムが利用できない、または正しくセットアップされていないため機能していない。");
	case WSAVERNOTSUPPORTED: return string("Winsock.dllのバージョンが範囲外である。\nWSAStartup()：要求したWinSockバージョンは、現在の実装ではサポートされていない。");
	case WSANOTINITIALISED: return string("WinSockシステムが初期化されていない。\nWinSock関数を実行しようとしたが、WSAStartup()が正常に実行されていないため機能しない。");
	case WSAEDISCON: return string("シャットダウン処理中。\n接続が相手の処理によって切断されようとしている。");
	case WSAHOST_NOT_FOUND: return string("ホストが見つからない。\nDNSなどの名前解決サービスが指定されたホストを見つけられなかった。プロトコルやサービスのクエリー操作においても返される。");
	case WSATRY_AGAIN: return string("指定されたホストが見つからない、またはサービスの異常。");
	case WSANO_RECOVERY: return string("回復不能なエラーが発生した。\nデータベース検索の場合は、名前解決サービスが使用できないことを意味する。");
	case WSANO_DATA: return string("要求されたタイプのデータレコードが見つからない。");
	}

	return "";
}
