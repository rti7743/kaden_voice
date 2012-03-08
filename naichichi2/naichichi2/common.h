#pragma once

#include <Winsock2.h>
#include <mswsock.h>

#include <windows.h>

#ifdef _MSC_VER // This check can be removed if you only build for Windows
#pragma warning (push)
#pragma warning (disable : 4005)
#include <intsafe.h>
#include <stdint.h>
#pragma warning (push)
#pragma warning (default : 4005)
#endif

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <list>
#include <functional>
//#include <Shellapi.h>
#include <assert.h>
#include <sstream>
#include "XLDebugUtil.h"
#include "sexytest.h"
//#include "sexyhook.h"

template<typename _INT> static const std::string num2str(_INT i)
{
	std::stringstream out;
	out << i;
	return out.str();
}
template<typename _INT> static const std::wstring num2wstr(_INT i)
{
	std::wstringstream out;
	out << i;
	return out.str();
}

//TRUE を true に
#define Btob(x)	((x) != FALSE)
//true を TRUE に
#define btoB(x)	((DWORD) x)


//newで割り当てたコンテナの削除
#define CDeleteAll(CONTAIR) { for(auto _rrr##__LINE__ = CONTAIR.begin() ; _rrr##__LINE__ != CONTAIR.end() ; ++_rrr##__LINE__){ delete *_rrr##__LINE__;	} CONTAIR.clear(); }
//foreach 何もしないか、 true を返すと継続。 false を返すとそこで break
#define CForeach(CONTAIR,lambda) {auto _rrr##__LINE__ = CONTAIR.begin(); auto _fff##__LINE__ = [&](decltype(*_rrr##__LINE__) _) -> bool { ##lambda## ; return true; }; for( ; _rrr##__LINE__ != CONTAIR.end() ; ++_rrr##__LINE__){ if (!  _fff##__LINE__(*_rrr##__LINE__) ) break;	} }
//巡回して削除。 標準とは逆なので注意。 何もしないか、true を返すとキープ false を返すと消す。 ループは一周するまで続ける。
#define CRemoveIF(CONTAIR,lambda) {auto _rrr##__LINE__ = CONTAIR.begin(); auto _fff##__LINE__ = [&](decltype(*_rrr##__LINE__) _) -> bool { ##lambda## ; return true; }; for( ; _rrr##__LINE__ != CONTAIR.end() ; ){ if (!  _fff##__LINE__(*_rrr##__LINE__) ) { _rrr##__LINE__ = CONTAIR.erase(_rrr##__LINE__);  }else{	++_rrr##__LINE__; } } }

class MainWindow;

#include "xreturn.h"
#include "methodcallback.h"
#include "windows_encoding.h"

#include <boost/thread.hpp>
#include <boost/asio.hpp>
