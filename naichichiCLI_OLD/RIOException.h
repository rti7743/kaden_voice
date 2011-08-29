// RIOException.h: RIOException クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RIOEXCEPTION_H__FDE1C551_6DA1_46C5_BD09_8780E543A18E__INCLUDED_)
#define AFX_RIOEXCEPTION_H__FDE1C551_6DA1_46C5_BD09_8780E543A18E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "comm.h"

#include "RException.h"

class RIOException : public RException  
{
public:
	RIOException()
	{
	}
	RIOException(std::string inErrorString)
	{
		setError(inErrorString);
	}
	RIOException(char* Format,...)
	{
		setError(Format,(char*)(&Format+1));
	}
	virtual ~RIOException()
	{
	}

};

#endif // !defined(AFX_RIOEXCEPTION_H__FDE1C551_6DA1_46C5_BD09_8780E543A18E__INCLUDED_)
