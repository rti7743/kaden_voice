// RConferException.h: RConferException クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RCONFEREXCEPTION_H__7B81D35A_7A9B_4CEA_9F8A_BA22F835B079__INCLUDED_)
#define AFX_RCONFEREXCEPTION_H__7B81D35A_7A9B_4CEA_9F8A_BA22F835B079__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "comm.h"
#include "RException.h"

/**
 * class RConferException:
 *
 * @author rti
 */
class RConferException : public RException  
{
public:
	RConferException()
	{
	}
	RConferException(unsigned long inErrorCode,std::string inErrorString)
	{
		setErrorCode(inErrorCode);
		setError(inErrorString);
	}
	RConferException(unsigned long inErrorCode,char* Format,...)
	{
		setErrorCode(inErrorCode);
		setError(Format,(char*)(&Format+1));
	}
	virtual ~RConferException()
	{
	}

	void setErrorCode(unsigned long	inErrorCode)
	{
		ErrorCode = inErrorCode;
	}
	unsigned long	getErrorCode()
	{
		return ErrorCode;
	}
private:
	//エラーコード.
	unsigned long	ErrorCode;
};

#endif // !defined(AFX_RCONFEREXCEPTION_H__7B81D35A_7A9B_4CEA_9F8A_BA22F835B079__INCLUDED_)
