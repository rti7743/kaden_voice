// XLFileUtil.h: XLFileUtil クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XLFILEUTIL_H__A51E414C_8F9A_46E5_A5CF_364F04C9FA00__INCLUDED_)
#define AFX_XLFILEUTIL_H__A51E414C_8F9A_46E5_A5CF_364F04C9FA00__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class XLFileUtil  
{
public:
	XLFileUtil();
	virtual ~XLFileUtil();

	//ファイルが存在するか?
	static bool XLFileUtil::Exist(const std::string & inFileName);
	//削除
	static bool XLFileUtil::unlink(const std::string & inFileName);
	static bool XLFileUtil::copy(const std::string & inFileNameA,const std::string & inFileNameB);
	static bool XLFileUtil::move(const std::string & inFileNameA,const std::string & inFileNameB);
	//ファイルをすべて string に読み込む.
	static std::string XLFileUtil::cat(const std::string & inFileName);
	//ファイルをすべて std::vector<char> に読み込む.
	static std::vector<char> XLFileUtil::cat_b(const std::string & inFileName);
	//ファイルをすべて out に読み込む
	static bool XLFileUtil::cat_b(const std::string & inFileName , std::vector<char>* out);

	//inStr を ファイルに書き込む
	static bool XLFileUtil::write(const std::string & inFileName,const std::string & inStr );
	//inBuffer を ファイルに書き込む
	static bool XLFileUtil::write(const std::string & inFileName,const std::vector<char> & inBuffer);
	//inBuffer を ファイルに書き込む
	static bool XLFileUtil::write(const std::string & inFileName,const char* data , int size);

};

#endif // !defined(AFX_XLFILEUTIL_H__A51E414C_8F9A_46E5_A5CF_364F04C9FA00__INCLUDED_)
