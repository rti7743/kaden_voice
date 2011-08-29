// RIOCannotWriteException.h: RIOCannotWriteException クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RIOCANNOTWRITEEXCEPTION_H__ECC52224_5E24_4EBE_AA81_452315017E2C__INCLUDED_)
#define AFX_RIOCANNOTWRITEEXCEPTION_H__ECC52224_5E24_4EBE_AA81_452315017E2C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "comm.h"

#include "RIOException.h"

class RIOCannotWriteException : public RIOException  
{
public:
	RIOCannotWriteException()
	{
	}
	RIOCannotWriteException(std::string inErrorString)
	{
		setError(inErrorString);
	}
	RIOCannotWriteException(char* Format,...)
	{
		setError(Format,(char*)(&Format+1));
	}
	virtual ~RIOCannotWriteException()
	{
	}

};

#endif // !defined(AFX_RIOCANNOTWRITEEXCEPTION_H__ECC52224_5E24_4EBE_AA81_452315017E2C__INCLUDED_)
