#pragma once

#ifdef _MSC_VER // This check can be removed if you only build for Windows
#include <Winsock2.h>
#include <mswsock.h>

#include <windows.h>

#pragma warning (push)
#pragma warning (disable : 4005)
#include <intsafe.h>
#include <stdint.h>
#pragma warning (push)
#pragma warning (default : 4005)
#else
#include <stdint.h>
#include <unistd.h>
#include <sys/sendfile.h>
#include <sys/types.h>
#include <dirent.h>
typedef unsigned int  LRESULT;
typedef unsigned int  WPARAM;
typedef unsigned int  LPARAM;
typedef unsigned int* HINSTANCE;
typedef unsigned int* HWND;
const int MAX_PATH = 255;

const int WM_CREATE                                             = 0x0001;
const int WM_DESTROY                                    = 0x0002;
const int WM_USER                                               = 0x0400;

#endif

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <list>
#include <functional>
#include <assert.h>
#include <sstream>
#include "sexytest.h"
//#include "sexyhook.h"
#include <boost/thread.hpp>
#include <boost/asio.hpp>

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
#include "windows_encoding.h"
#include "XLDebugUtil.h"

static std::string mapfind(const std::map<std::string,std::string>& stringmap ,const std::string& name ,const std::string& def = "" )
{
	auto it = stringmap.find(name);
	if (it == stringmap.end()) return def;
	return it->second;
}
