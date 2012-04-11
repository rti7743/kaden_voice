// XLUrlParser.h: XLUrlParser クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XLURLPARSER_H__3E362401_E16F_401B_A2CB_9B409C4F79D8__INCLUDED_)
#define AFX_XLURLPARSER_H__3E362401_E16F_401B_A2CB_9B409C4F79D8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class XLUrlParser  
{
public:
	XLUrlParser();
	virtual ~XLUrlParser();

	void Parse(const std::string & inURL);

	std::string getProtocol() const{	return this->Protocol;	}
	std::string getUserName() const{	return this->UserName;	}
	std::string getPassword() const{	return this->Password;	}
	std::string getHost() const{	return this->Host;	}
	int getPort() const{	return this->Port;	}
	std::string getPath() const{	return this->Path;	}

	static void test();

private:
	std::string		Protocol;
	std::string		UserName;
	std::string		Password;
	std::string		Host;
	int				Port;
	std::string		Path;
};

#endif // !defined(AFX_XLURLPARSER_H__3E362401_E16F_401B_A2CB_9B409C4F79D8__INCLUDED_)
