// XLHttpHeader.h: XLHttpHeader クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XLHTTPHEADER_H__3569AE9A_5235_4741_B71B_1FB2ABE4CD70__INCLUDED_)
#define AFX_XLHTTPHEADER_H__3569AE9A_5235_4741_B71B_1FB2ABE4CD70__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XLStringUtil.h"

class XLHttpHeader  
{
public:
	XLHttpHeader();
	virtual ~XLHttpHeader();

	bool Parse(const char * inHeader );
	bool Parse(const std::string& inHeader )
	{
		return this->Parse(inHeader.c_str()  );
	}

	std::string getRequestMethod() const
	{
		return this->getFirstParam(0);
	}
	std::string getRequestPath() const
	{
		return this->getFirstParam(1);
	}
	std::string getRequestProtocol() const
	{
		return this->getFirstParam(2);
	}

	std::string getResponsProtocol() const
	{
		return this->getFirstParam(0);	//HTTP/1.0
	}
	std::string getResponsResultCode() const
	{
		return this->getFirstParam(1);	//200
	}
	std::string getResponsResultState() const
	{
		return this->getFirstParam(2);	//OK
	}

	std::string getFirstParam(int inNumebr) const;

	void setFirstParam(int inNumebr, const std::string & inStr)
	{
		assert(inNumebr >= 0 && inNumebr <= 2);
		this->FirstHeader[inNumebr] = inStr;
	}
	void setFirstParams( const std::string & inStr1, const std::string & inStr2, const std::string & inStr3)
	{
		this->FirstHeader[0] = inStr1;
		this->FirstHeader[1] = inStr2;
		this->FirstHeader[2] = inStr3;
	}
	unsigned int getHeaderSize() const
	{
		return this->HeaderSize;
	}

	void setAt(const std::string inKey , const std::string & inValue);
	std::string getAt(const std::string inKey) const;
	std::string Build() const;
	const std::map<std::string,std::string>	getHeader() const
	{
		return this->Header;
	}
	const std::map<std::string,std::string>	getGet() const
	{
		return this->Get;
	}
	const std::map<std::string,std::string>	getPost()const
	{
		return this->Post;
	}
	const std::map<std::string,std::string>	getRequest() const
	{
		return XLStringUtil::merge(this->Get,this->Post,true);
	}

	static void test();

private:
	std::string							FirstHeader[3];
	std::map<std::string,std::string>	Header;
	std::map<std::string,std::string>	Get;
	std::map<std::string,std::string>	Post;
	unsigned int HeaderSize;
};

#endif // !defined(AFX_XLHTTPHEADER_H__3569AE9A_5235_4741_B71B_1FB2ABE4CD70__INCLUDED_)
