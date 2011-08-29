// ROutOfRangeException.h: ROutOfRangeException クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ROUTOFRANGEEXCEPTION_H__2E93FFED_B7EA_49BB_9329_B9ECFA6BEA3E__INCLUDED_)
#define AFX_ROUTOFRANGEEXCEPTION_H__2E93FFED_B7EA_49BB_9329_B9ECFA6BEA3E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class ROutOfRangeException;

#include "comm.h"

#include "RException.h"

class ROutOfRangeException : public RException  
{
public:
	ROutOfRangeException()
	{
	}
	ROutOfRangeException(std::string inErrorString)
	{
		setError(inErrorString);
	}
	ROutOfRangeException(char* Format,...)
	{
		setError(Format,(char*)(&Format+1));
	}
	virtual ~ROutOfRangeException()
	{
	}

};

#endif // !defined(AFX_ROUTOFRANGEEXCEPTION_H__2E93FFED_B7EA_49BB_9329_B9ECFA6BEA3E__INCLUDED_)
