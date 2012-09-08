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
	static bool Exist(const std::string & inFileName);
	//削除
	static bool del(const std::string & inFileName);
	static bool copy(const std::string & inFileNameA,const std::string & inFileNameB);
	static std::string pwd();
	static bool findfile(const std::string & dir,const std::function< bool(const std::string& filename,const std::string& fullfilename) >& callback);

	static bool move(const std::string & inFileNameA,const std::string & inFileNameB);
	//ファイルをすべて string に読み込む.
	static std::string cat(const std::string & inFileName);
	//ファイルをすべて std::vector<char> に読み込む.
	static std::vector<char> cat_b(const std::string & inFileName);
	//ファイルをすべて out に読み込む
	static bool cat_b(const std::string & inFileName , std::vector<char>* out);

	//inStr を ファイルに書き込む
	static bool write(const std::string & inFileName,const std::string & inStr );
	//inBuffer を ファイルに書き込む
	static bool write(const std::string & inFileName,const std::vector<char> & inBuffer);
	//inBuffer を ファイルに書き込む
	static bool write(const std::string & inFileName,const char* data , int size);

	static  size_t getfilesize(const std::string & inFileName);

	static  time_t getfiletime(const std::string & inFileName);
};

#endif // !defined(AFX_XLFILEUTIL_H__A51E414C_8F9A_46E5_A5CF_364F04C9FA00__INCLUDED_)
