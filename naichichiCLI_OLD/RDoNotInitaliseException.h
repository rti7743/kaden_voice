// RDoNotInitaliseException.h: RDoNotInitaliseException クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RDONOTINITALISEEXCEPTION_H__91A9C6EE_FE0C_44D1_A05D_8AC8AEF8DC82__INCLUDED_)
#define AFX_RDONOTINITALISEEXCEPTION_H__91A9C6EE_FE0C_44D1_A05D_8AC8AEF8DC82__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class RDoNotInitaliseException;

#include "comm.h"


#include "RException.h"

//初期化が失敗したときに呼び出される例外
class RDoNotInitaliseException : public RException  
{
public:
	RDoNotInitaliseException()
	{
	}
	RDoNotInitaliseException(std::string inErrorString)
	{
		setError(inErrorString);
	}
	RDoNotInitaliseException(char* Format,...)
	{
		setError(Format,(char*)(&Format+1));
	}
	virtual ~RDoNotInitaliseException()
	{
	}

};

#endif // !defined(AFX_RDONOTINITALISEEXCEPTION_H__91A9C6EE_FE0C_44D1_A05D_8AC8AEF8DC82__INCLUDED_)
