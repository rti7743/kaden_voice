// RIOCannotCloseException.h: RIOCannotCloseException クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RIOCANNOTCLOSEEXCEPTION_H__85EF6735_59F4_4BD8_9A1A_C7A292BA8DB4__INCLUDED_)
#define AFX_RIOCANNOTCLOSEEXCEPTION_H__85EF6735_59F4_4BD8_9A1A_C7A292BA8DB4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "comm.h"

#include "RIOException.h"

class RIOCannotCloseException : public RIOException  
{
public:
	RIOCannotCloseException()
	{
	}
	RIOCannotCloseException(std::string inErrorString)
	{
		setError(inErrorString);
	}
	RIOCannotCloseException(char* Format,...)
	{
		setError(Format,(char*)(&Format+1));
	}
	virtual ~RIOCannotCloseException()
	{
	}

};

#endif // !defined(AFX_RIOCANNOTCLOSEEXCEPTION_H__85EF6735_59F4_4BD8_9A1A_C7A292BA8DB4__INCLUDED_)
