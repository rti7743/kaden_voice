// RWin32Exception.h: RWin32Exception クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RWIN32EXCEPTION_H__9CC056B0_31D9_4906_AC1E_A308063630DD__INCLUDED_)
#define AFX_RWIN32EXCEPTION_H__9CC056B0_31D9_4906_AC1E_A308063630DD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "comm.h"
#include "RConferException.h"

//Windows API が出したエラーをフックします。
class RWin32Exception : public RConferException  
{
public:
	RWin32Exception(const string inFormat);
	RWin32Exception(char* Format,...);
	RWin32Exception(unsigned long inErrorCode,std::string inErrorString)
	{
		setErrorCode(inErrorCode);
		setError(inErrorString);
	}
	RWin32Exception(unsigned long inErrorCode,char* Format,...)
	{
		setErrorCode(inErrorCode);
		setError(Format,(char*)(&Format+1));
	}
	virtual ~RWin32Exception();
private:
	void AppendWin32Error(DWORD inErrorCode);

};

#endif // !defined(AFX_RWIN32EXCEPTION_H__9CC056B0_31D9_4906_AC1E_A308063630DD__INCLUDED_)
