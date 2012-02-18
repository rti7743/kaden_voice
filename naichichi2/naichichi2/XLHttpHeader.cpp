// XLHttpHeader.cpp: XLHttpHeader クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "common.h"
#include "XLHttpHeader.h"
#include "XLStringUtil.h"

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

XLHttpHeader::XLHttpHeader()
{
	this->HeaderSize = 0;
}

XLHttpHeader::~XLHttpHeader()
{

}

bool XLHttpHeader::Parse(const char * inHeader)
{
	this->HeaderSize = 0;

	const char * p = inHeader;
	//最初のヘッダー
	{
		int firstHeadSpaceCount = 0;
		for( const char * start = p; *p ; ++p )
		{
			if (*p == '\r' || *p == '\n')
			{
				this->FirstHeader[2] = std::string(start , 0 , (int)(p - start));
				if (*p == '\r' && *(p+1) == '\n')
				{
					p++;
				}
				break;
			}
			else if (*p == ' ')
			{
				if (firstHeadSpaceCount >= 2)
				{
					continue;
				}

				this->FirstHeader[firstHeadSpaceCount] = std::string(start , 0 , (int)(p - start));
				firstHeadSpaceCount++;
				start = p + 1;
			}
		}
	}

	//2行位以降のヘッダー
	while(*p)
	{
		//改行までシーク.
		const char * sep = NULL;
		const char * value_start = NULL;
		for( const char * start = p; *p ; ++p )
		{
			//複数行にまたがっているヘッダーは考慮しないことにするw
			if (*p == '\r' || *p == '\n')
			{
				if (p == start || sep == NULL || value_start == NULL)
				{
					break;
				}
				std::string key = std::string(start , 0, (int)(sep - start));
				std::string value = std::string(value_start , 0, (int)(value_start - p));
				if (Header.find(key) == Header.end())
				{
					this->Header[key] = value;
				}
				else
				{
					this->Header[key] += value;
				}

				if (*p == '\r' && *(p+1) == '\n')
				{
					p++;
				}
				break;
			}
			else if (sep == NULL && *p == ':')
			{
				sep = p;
			}
			else if (sep != NULL && value_start == NULL && (*p != ' ' && *p != '\t'))
			{
				value_start = p;
			}
		}

		//次のヘッダへ
		p++;

		//ヘッダー終端チェック
		if (*p == '\r' || *p == '\n')
		{
			if (*p == '\r' && *(p+1) == '\n')
			{
				p++;
			}
			this->HeaderSize = (unsigned int)(p - inHeader);
		}
	}

	//GETパース
	{
		int getsep = this->FirstHeader[1].find("?");
		if (getsep >= 0)
		{
			this->Get = XLStringUtil::crosssplit("&","=",this->FirstHeader[1].substr(getsep+1) );
			this->FirstHeader[1] = this->FirstHeader[1].substr(0,getsep);
		}
	}
	//POSTパース
	{
		if ( XLStringUtil::strtoupper( this->FirstHeader[0] ) == "POST")
		{
			this->Post = XLStringUtil::crosssplit("&","=",p);
		}
	}

	return true;
}

void XLHttpHeader::setAt(const std::string inKey , const std::string & inValue)
{
	this->Header[inKey] = inValue;
}

std::string XLHttpHeader::getAt(const std::string inKey) const
{
	auto i = this->Header.find(inKey);
	return i->second;
}

std::string XLHttpHeader::Build() const
{
	//最初のライン.
	std::string ret = this->FirstHeader[0]  + " " + this->FirstHeader[1]  + " " + this->FirstHeader[2] + "\r\n";
	//それ以降.
	auto i = this->Header.begin();
	auto e = this->Header.end();

	for( ; i != e ; i ++)
	{
		ret += i->first + ": " + i->second + "\r\n";
	}
	ret += "\r\n";

	return ret;
}


std::string XLHttpHeader::getFirstParam(int inNumebr) const
{
	assert(inNumebr < 3);
	return this->FirstHeader[inNumebr];
}

/*
void XLHttpHeader::test()
{
	{
		XLHttpHeader t;
		std::string th = 
				"HTTP/1.1 200 Document follows\r\n"
				"MIME-Version: 1.0\r\n"
				"Server: AnWeb/1.42n\r\n"
				"Date: Sat, 12 Mar 2005 05:45:20 GMT\r\n"
				"Content-Type: text/html\r\n"
				"Transfer-Encoding: chunked\r\n"
				"\r\n";
		bool r;
		assert( ( r = t.Parse(th.c_str() , th.size() - 1) ) == true );

		string rr;
		assert( ( rr = t.getResponsProtocol() ) == "HTTP/1.1" );
		assert( ( rr = t.getResponsResultCode() ) == "200" );
		assert( ( rr = t.getResponsResultState() ) == "Document follows" );
		assert( ( rr = t.getAt("Date") ) == "Sat, 12 Mar 2005 05:45:20 GMT" );
		assert( ( rr = t.getAt("MIME-Version") ) == "1.0" );
		assert( ( rr = t.getAt("Content-Type") ) == "text/html" );
		assert( ( rr = t.getAt("Transfer-Encoding") ) == "chunked" );
	}
}

*/