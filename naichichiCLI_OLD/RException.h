// RException.h: RException クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REXCEPTION_H__54EBFB0B_4F4C_43B5_8597_6DAB5B93B3F0__INCLUDED_)
#define AFX_REXCEPTION_H__54EBFB0B_4F4C_43B5_8597_6DAB5B93B3F0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class RException;

#include "comm.h"


//すべての例外の基底クラス
class RException  
{
private:
	//エラー報告に使う文字列
	std::string		ErrorString;
protected:
	//エラーをセット
	void setError(std::string inErrorString)
	{
		ErrorString = inErrorString;
	}
	//エラーをセット
	void setError(const char* Format, va_list arg_ptr)
	{
		char buffer[1024];
		_vsnprintf(buffer,1024,(char*)Format,arg_ptr);
		ErrorString = buffer;
	}
	//エラーをセット
	void setError(const char* Format,...)
	{
		char buffer[1024];
		_vsnprintf(buffer,1024,Format,(char*)(&Format+1));
		ErrorString = buffer;
	}
public:
	RException()
	{
	}
	RException(std::string inErrorString)
	{
		setError(inErrorString);
	}
	RException(char* Format,...)
	{
		setError(Format,(char*)(&Format+1));
	}
	virtual ~RException()
	{
	}

	//エラーメッセージの取得
	virtual const char* getMessage()
	{
		return ErrorString.c_str();
	}

};

#endif // !defined(AFX_REXCEPTION_H__54EBFB0B_4F4C_43B5_8597_6DAB5B93B3F0__INCLUDED_)
