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
	for(auto it = this->Files.begin() ; it != this->Files.end() ; ++it)
	{
		delete it->second;
	}
}

bool XLHttpHeader::Parse(const char * inHeader , int size)
{
	_USE_WINDOWS_ENCODING;
	this->HeaderSize = 0;

	const char * p = inHeader;
	const char * endP = inHeader + size;
	//最初のヘッダー
	{
		int firstHeadSpaceCount = 0;
		for( const char * start = p; p < endP; ++p )
		{
			if (*p == '\r' || *p == '\n')
			{
				this->FirstHeader[2] = XLStringUtil::strtoupper( std::string(start , 0 , (int)(p - start)) );
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
		for( const char * start = p; p < endP ; ++p )
		{
			//複数行にまたがっているヘッダーは考慮しないことにするw
			if (*p == '\r' || *p == '\n')
			{
				if (p == start || sep == NULL || value_start == NULL)
				{
					break;
				}
				std::string key = XLStringUtil::strtolower( std::string(start , 0, (int)(sep - start)) );
				std::string value = std::string(value_start , 0, (int)(p - value_start));
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
				p+=2;
			}
			else
			{
				p++;
			}
			this->HeaderSize = (unsigned int)(p - inHeader) ;
			break;
		}
	}

	//GETパース
	{
		int getsep = this->FirstHeader[1].find("?");
		if (getsep >= 0)
		{
			std::string str = this->FirstHeader[1].substr(getsep+1);
			str = XLStringUtil::urldecode( str );
			str = _U2A(str.c_str());

			this->Get = XLStringUtil::crosssplit("&","=",str );
			this->FirstHeader[1] = this->FirstHeader[1].substr(0,getsep);
		}
	}
	return true;
}

bool XLHttpHeader::MultipartBounderParse( const char * inBody , int size )
{
	const std::string contenttype =  getAt("content-type");
	if (! strstr(contenttype.c_str() , "multipart/form-data")  )
	{
		return false;
	}

	//普通じゃないPOSTパース multipart/form-data
	const char * bounder = strstr( contenttype.c_str() , "boundary=");
	if (!bounder)
	{
		this->Post.clear();
	}
	else
	{
		bounder += (sizeof("boundary=") - 1);
		std::string fullbounder = std::string("--") + bounder; //bounder は -- を余計につける
		const char * endP = inBody + size;
		const char * p = inBody;
		while(p < endP)
		{
			const char * nextbounder = strstr(p , fullbounder.c_str() );
			if (!nextbounder)
			{//bounder がない
				nextbounder = endP;
			}
			if (nextbounder > p)
			{
				// p ～ nextbounder の間の解析
				std::map<std::string,std::string> innerheader;
				while(p < nextbounder)
				{
					//改行までシーク.
					const char * sep = NULL;
					const char * value_start = NULL;
					for( const char * start = p; p < nextbounder ; ++p )
					{
						//複数行にまたがっているヘッダーは考慮しないことにするw
						if (*p == '\r' || *p == '\n')
						{
							if (p == start || sep == NULL || value_start == NULL)
							{
								break;
							}
							std::string key = XLStringUtil::strtolower( std::string(start , 0, (int)(sep - start)) );
							std::string value = std::string(value_start , 0, (int)(p - value_start));
							if (innerheader.find(key) == innerheader.end())
							{
								innerheader[key] = value;
							}
							else
							{
								innerheader[key] += value;
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
							p+=2;
						}
						else
						{
							p++;
						}
						break;
					}
				}
				//content-disposition から、 name と filename を取得する.
				auto contentDispositionMap = XLStringUtil::crosssplitChop(";","=", innerheader["content-disposition"] ); 
				const std::string contentDispositionName = XLStringUtil::dequote( mapfind(contentDispositionMap,"name","nanachisan") );

				auto alreadyit = this->Files.find(contentDispositionName);
				if (alreadyit != this->Files.end())
				{
					delete alreadyit->second;
					this->Files.erase(alreadyit);
				}

				__file_struct* filestruct = new __file_struct;
				filestruct->data.insert(filestruct->data.end() , p, nextbounder);
				filestruct->filename = XLStringUtil::dequote( mapfind(contentDispositionMap,"filename","nanashifile") );
				filestruct->mime = innerheader["content-type"];

				this->Files[contentDispositionName] = filestruct;
			}

			//次のbounderを探す
			p = nextbounder + fullbounder.size();
			if (p > endP -2)
			{
				break;
			}
			if (p[0] == '-' && p[1] == '-')
			{//終端
				break;
			}
			if (p[0] == '\r')
			{
				p ++;
				if (p[0] == '\n') p++;
			}
			else if (p[0] == '\n')
			{
				p ++;
			}
			else
			{//よくわからない bounder!!
				break;
			}
		}
	}
	return ! this->Files.empty();
}

bool XLHttpHeader::PostParse( const char * inBody , int size )
{
	_USE_WINDOWS_ENCODING;

	if ( ! MultipartBounderParse(inBody,size) )
	{
		//普通のPOSTパース
		std::string str = std::string(inBody,0,size);
		str = XLStringUtil::urldecode( str );
		str = _U2A(str.c_str());

		this->Post = XLStringUtil::crosssplit("&","=",str );
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
	if (i == this->Header.end() ) return "";
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