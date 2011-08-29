// RIOCannotOpenException.h: RIOCannotOpenException クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RIOCANNOTOPENEXCEPTION_H__758F44CA_DFE6_405F_AAD7_A425B73DACE0__INCLUDED_)
#define AFX_RIOCANNOTOPENEXCEPTION_H__758F44CA_DFE6_405F_AAD7_A425B73DACE0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "comm.h"

#include "RIOException.h"

class RIOCannotOpenException : public RIOException  
{
public:
	RIOCannotOpenException()
	{
	}
	RIOCannotOpenException(std::string inErrorString)
	{
		setError(inErrorString);
	}
	RIOCannotOpenException(char* Format,...)
	{
		setError(Format,(char*)(&Format+1));
	}
	virtual ~RIOCannotOpenException()
	{
	}

};

#endif // !defined(AFX_RIOCANNOTOPENEXCEPTION_H__758F44CA_DFE6_405F_AAD7_A425B73DACE0__INCLUDED_)
