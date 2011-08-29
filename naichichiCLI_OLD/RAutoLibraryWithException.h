// RAutoLibraryWithException.h: RAutoLibraryWithException クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RAUTOLIBRARYWITHEXCEPTION_H__BFCDEDC4_9433_4519_B572_424984DF4C5D__INCLUDED_)
#define AFX_RAUTOLIBRARYWITHEXCEPTION_H__BFCDEDC4_9433_4519_B572_424984DF4C5D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "comm.h"

#include "RDoNotInitaliseException.h"
#include "RNotSupportException.h"
#include "RWin32Exception.h"

//RAutoLibrary  に例外処理をプラスしたライブラリ。
//要求聞いてくれないとException投げて「うおーん」と泣いてドアに蹴りいれちゃうゾ(厨かYO!)
class RAutoLibraryWithException
{
public:
	//構築
	RAutoLibraryWithException() throw()
	{
		LabraryHandle = NULL;
	}

	//指定した名前で構築
	//失敗は RDoNotInitaliseException です。
	RAutoLibraryWithException(const char* inDLLName) throw(RDoNotInitaliseException)
	{
		Create(inDLLName);
	}

	//破棄
	virtual ~RAutoLibraryWithException()
	{
		Destory();
	}

	//DLL設定
	//@inDLLName	DLLの名前
	//失敗は RDoNotInitaliseException です。
	void Create(const char* inDLLName) throw(RDoNotInitaliseException)
	{
		Destory();

#ifndef _WIN32_WCE
		LabraryHandle = ::LoadLibrary(inDLLName);
#else
		MY_USES_CONVERSION;
		LabraryHandle = ::LoadLibrary(_A2W(inDLLName) );
#endif
		if (!IsReading())
		{
			try
			{
				throw RWin32Exception("RAutoLibraryWithException:%s の読み込みに失敗しますた",inDLLName);
			}
			catch(RWin32Exception e)
			{
				throw RDoNotInitaliseException(e.getMessage());
			}
		}
	}
	//DLL破棄
	void Destory()
	{
		if (!IsReading()) return;
		::FreeLibrary((HINSTANCE)LabraryHandle);
	}	

	//DLLから 関数を取得
	//@inFunctionName		関数名
	//初期化かされていない場合は RDoNotInitaliseException です。
	//そんな関数ない場合は RNotSupportException です。
	FARPROC GetProcAddress(const char* inFunctionName) throw(RDoNotInitaliseException,RNotSupportException)
	{
		if (!IsReading()) 
		{
			throw RDoNotInitaliseException("RAutoLibraryWithException:%s 関数を読み込めといわれましたが、初期化すらされてません",inFunctionName);
		}
#ifndef _WIN32_WCE
		FARPROC ret = ::GetProcAddress((HINSTANCE)LabraryHandle, inFunctionName);
#else
		MY_USES_CONVERSION;
		FARPROC ret = ::GetProcAddress((HINSTANCE)LabraryHandle, _A2W(inFunctionName) );
#endif
		if (ret == NULL)
		{
			throw RNotSupportException("RAutoLibraryWithException:%s 関数はサポートされていません",inFunctionName);
		}
		return ret;
	}
	//DLLは読み込まれているの?
	bool IsReading()
	{
		return LabraryHandle != NULL;
	}

private:
	HANDLE	LabraryHandle;
};

#endif // !defined(AFX_RAUTOLIBRARYWITHEXCEPTION_H__BFCDEDC4_9433_4519_B572_424984DF4C5D__INCLUDED_)
