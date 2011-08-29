// RNotSupportException.h: RNotSupportException クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RNOTSUPPORTEXCEPTION_H__6659B25E_536A_4477_A91C_DB122095DF33__INCLUDED_)
#define AFX_RNOTSUPPORTEXCEPTION_H__6659B25E_536A_4477_A91C_DB122095DF33__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class RNotSupportException;

#include "comm.h"

#include "RException.h"


//サポートされていない機能が呼び出されたときに返す例外.
class RNotSupportException : public RException  
{
public:
	RNotSupportException()
	{
	}
	RNotSupportException(std::string inErrorString)
	{
		setError(inErrorString);
	}
	RNotSupportException(char* Format,...)
	{
		setError(Format,(char*)(&Format+1));
	}
	virtual ~RNotSupportException()
	{
	}

};

#endif // !defined(AFX_RNOTSUPPORTEXCEPTION_H__6659B25E_536A_4477_A91C_DB122095DF33__INCLUDED_)
