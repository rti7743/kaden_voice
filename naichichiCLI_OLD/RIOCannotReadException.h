// RIOCannotReadException.h: RIOCannotReadException クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RIOCANNOTREADEXCEPTION_H__C65BEA54_E811_4BD5_9216_A6DD20DF7AD0__INCLUDED_)
#define AFX_RIOCANNOTREADEXCEPTION_H__C65BEA54_E811_4BD5_9216_A6DD20DF7AD0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "comm.h"

#include "RIOException.h"

class RIOCannotReadException : public RIOException  
{
public:
	RIOCannotReadException()
	{
	}
	RIOCannotReadException(std::string inErrorString)
	{
		setError(inErrorString);
	}
	RIOCannotReadException(char* Format,...)
	{
		setError(Format,(char*)(&Format+1));
	}
	virtual ~RIOCannotReadException()
	{
	}

};

#endif // !defined(AFX_RIOCANNOTREADEXCEPTION_H__C65BEA54_E811_4BD5_9216_A6DD20DF7AD0__INCLUDED_)
