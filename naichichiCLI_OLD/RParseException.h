// RParseException.h: RParseException クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RPARSEEXCEPTION_H__18C73742_8358_4694_95AD_A57FF130BEC5__INCLUDED_)
#define AFX_RPARSEEXCEPTION_H__18C73742_8358_4694_95AD_A57FF130BEC5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RException.h"

class RParseException : public RException  
{
public:
	RParseException()
	{
	}
	RParseException(std::string inErrorString)
	{
		setError(inErrorString);
	}
	RParseException(char* Format,...)
	{
		setError(Format,(char*)(&Format+1));
	}
	virtual ~RParseException()
	{
	}
};

#endif // !defined(AFX_RPARSEEXCEPTION_H__18C73742_8358_4694_95AD_A57FF130BEC5__INCLUDED_)
