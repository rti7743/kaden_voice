// RAlreadyException.h: RAlreadyException クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RALREADYEXCEPTION_H__C04D7802_52C4_49DC_A568_16B6955E1A50__INCLUDED_)
#define AFX_RALREADYEXCEPTION_H__C04D7802_52C4_49DC_A568_16B6955E1A50__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class RNotFoundException;
 
#include "comm.h"

#include "RException.h"


//すでにあるときに呼ばれる
class RAlreadyException : public RException  
{
public:
	RAlreadyException()
	{
	}
	RAlreadyException(std::string inErrorString)
	{
		setError(inErrorString);
	}
	RAlreadyException(char* Format,...)
	{
		setError(Format,(char*)(&Format+1));
	}
	virtual ~RAlreadyException()
	{
	}

};

#endif // !defined(AFX_RALREADYEXCEPTION_H__C04D7802_52C4_49DC_A568_16B6955E1A50__INCLUDED_)
