// XLFileUtil.cpp: XLFileUtil クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "common.h"
#include "XLFileUtil.h"
#include "XLStringUtil.h"

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

XLFileUtil::XLFileUtil()
{

}

XLFileUtil::~XLFileUtil()
{

}


//ファイルが存在するか?
bool XLFileUtil::Exist(const std::string & inFileName)
{
	return (0xFFFFFFFF != GetFileAttributes(XLStringUtil::pathseparator(inFileName).c_str() ));
}

//削除
bool XLFileUtil::unlink(const std::string & inFileName)
{
	::unlink(XLStringUtil::pathseparator(inFileName).c_str());
	return true;
}
bool XLFileUtil::copy(const std::string & inFileNameA,const std::string & inFileNameB)
{
//	::copy(inFileNameA.c_str(),inFileNameB.c_str());
	::CopyFile(XLStringUtil::pathseparator(inFileNameA).c_str(),XLStringUtil::pathseparator(inFileNameB).c_str(),TRUE);
	return true;
}
bool XLFileUtil::move(const std::string & inFileNameA,const std::string & inFileNameB)
{
	::rename(XLStringUtil::pathseparator(inFileNameA).c_str(),XLStringUtil::pathseparator(inFileNameB).c_str());
	return true;
}


//ファイルをすべて std::string に読み込む.
std::string XLFileUtil::cat(const std::string & inFileName)
{
	std::vector<char> out;
	FILE * fp = fopen(inFileName.c_str() , "rb");
	//存在しない場合は空
	if (fp == NULL) return "";

	//ケツに持っていって.
	fseek(fp , 0 ,SEEK_END);

	//これでサイズがわかる.
	unsigned long size = ftell(fp);

	//先頭に戻す.
	fseek(fp , 0 ,SEEK_SET);

	//領域を確保して読み込む
	out.resize(size + 1);
	fread( &out[0] , 1 , size  , fp);
	out[size] = '0';	//終端.

	fclose(fp);

	return (const char*) (&out[0]);
}

//ファイルをすべて std::vector<char> に読み込む.
std::vector<char> XLFileUtil::cat_b(const std::string & inFileName)
{
	std::vector<char> out;
	cat_b(inFileName,&out);
	return out;
}

//ファイルをすべて std::vector<char> に読み込む.
bool XLFileUtil::cat_b(const std::string & inFileName , std::vector<char>* out)
{
	out->clear();

	FILE * fp = fopen(inFileName.c_str() , "rb");
	//存在しない場合は空
	if (fp == NULL) return false;

	//ケツに持っていって.
	fseek(fp , 0 ,SEEK_END);

	//これでサイズがわかる.
	unsigned long size = ftell(fp);

	//先頭に戻す.
	fseek(fp , 0 ,SEEK_SET);

	//領域を確保して読み込む
	out->resize(size);
	fread( &((*out)[0]) , 1 , size  , fp);
	
	fclose(fp);

	return true;
}


//inStr を ファイルに書き込む
bool XLFileUtil::write(const std::string & inFileName,const std::string & inStr )
{
	return write( inFileName , inStr.c_str() , inStr.size());
}

//inBuffer を ファイルに書き込む
bool XLFileUtil::write(const std::string & inFileName,const std::vector<char> & inBuffer)
{
	return write(inFileName,&inBuffer[0] ,  inBuffer.size());
}
//inBuffer を ファイルに書き込む
bool XLFileUtil::write(const std::string & inFileName,const char* data , int size)
{
	FILE * fp = fopen(inFileName.c_str() , "wb");
	if (fp == NULL)
	{
		assert(0);
		return false;
	}

	fwrite( data , 1, size  , fp);

	fclose(fp);
	return true;
}