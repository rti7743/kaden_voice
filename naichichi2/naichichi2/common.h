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
#define DeleteAll(CONTAIR) { for(auto _rrr##__LINE__ = CONTAIR.begin() ; _rrr##__LINE__ != CONTAIR.end() ; ++_rrr##__LINE__){ delete *_rrr##__LINE__;	} CONTAIR.clear(); }

class MainWindow;

#include "xreturn.h"
#include "methodcallback.h"
#include "windows_encoding.h"

#include <boost/thread.hpp>
#include <boost/asio.hpp>
