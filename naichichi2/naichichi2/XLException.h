#pragma once

#ifdef _MSC_VER
#endif
#include <string>
#include <typeinfo>
#include <iostream>
#include <sstream>

//UTF-8 BOMいれた
//xreturn::error として使ってね。
class XLException : public std::exception {
protected:
	std::string message; //
	int code;			 //std::exceptionがなぜか保持してくれないので自分で保持する.
public:

	XLException() : code(0)
	{
	}
	XLException(const std::string & message , int code)
		: std::exception() , message(message) , code(code)
	{
	}
	XLException(const std::string & message)
		: std::exception() , message(message) , code(0)
	{
	}
	XLException(const XLException& error)
		: std::exception() 
	{
		this->message = error.message;
		this->code = error.code;
	}
	XLException(const XLException& error,const std::string & message)
		: std::exception()
	{
		this->message = message + error.message;
		this->code = error.code;
	}
	XLException(const XLException& error,const std::string & message,int code)
		: std::exception()
	{
		this->message = message + error.message;
		this->code = code;
	}
	virtual ~XLException() throw()
	{
	}

	virtual int getErrorCode() const
	{
		return this->code;
	}
	virtual std::string getErrorMessage() const
	{
		return this->message;
	}
	virtual std::string getFullErrorMessage() const
	{
		std::stringstream out;
		out << "===message===" << std::endl;
		out << this->message << std::endl << std::endl;
		out << "===code===" << std::endl;
		out << this->code << std::endl << std::endl;

		return out.str();
	}
};
