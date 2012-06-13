// XLFileUtil.cpp: XLFileUtil クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "common.h"
#include "XLFileUtil.h"
#include "XLStringUtil.h"

#if _MSC_VER
#else
#endif

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
	const auto a = XLStringUtil::pathseparator(inFileName);
#if _MSC_VER
	return (0xFFFFFFFF != GetFileAttributes(a.c_str() ));
#else
	struct stat st;
	return stat(a.c_str() ,&st)  == 0;
#endif
}

//削除
bool XLFileUtil::del(const std::string & inFileName)
{
	const auto filename = XLStringUtil::pathseparator(inFileName);
	::unlink(filename.c_str());
	return true;
}


bool XLFileUtil::copy(const std::string & inFileNameA,const std::string & inFileNameB)
{
	const auto a = XLStringUtil::pathseparator(inFileNameA);
	const auto b = XLStringUtil::pathseparator(inFileNameB);
#if _MSC_VER
	::CopyFile(a.c_str(),b.c_str(),TRUE);
#else
	int read_fd;
	int write_fd;
	struct stat stat_buf;
	off_t offset = 0;

	/* Open the input file. */
	read_fd = open (a.c_str(), O_RDONLY);
	if (!read_fd)
	{
		return false;
	}

	/* Stat the input file to obtain its size. */
	fstat (read_fd, &stat_buf);
	/* Open the output file for writing, with the same permissions as the
	 * source file. */
	write_fd = open (b.c_str(), O_WRONLY | O_CREAT, stat_buf.st_mode);
	if (!write_fd)
	{
		close (read_fd);
		return false;
	}
	/* Blast the bytes from one file to the other. */
	sendfile (write_fd, read_fd, &offset, stat_buf.st_size);
	/* Close up. */
	close (read_fd);
	close (write_fd);
#endif
	return true;
}
bool XLFileUtil::move(const std::string & inFileNameA,const std::string & inFileNameB)
{
	const auto a = XLStringUtil::pathseparator(inFileNameA);
	const auto b = XLStringUtil::pathseparator(inFileNameB);

	::rename(a.c_str(),b.c_str());
	return true;
}


//ファイルをすべて std::string に読み込む.
std::string XLFileUtil::cat(const std::string & inFileName)
{
	const auto filename = XLStringUtil::pathseparator(inFileName);

	std::vector<char> out;
	FILE * fp = fopen(filename.c_str() , "rb");
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
	const auto filename = XLStringUtil::pathseparator(inFileName);

	FILE * fp = fopen(filename.c_str() , "rb");
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

std::string XLFileUtil::pwd()
{
#if _MSC_VER
	char buffer[MAX_PATH] = {0};
        GetCurrentDirectory(MAX_PATH , buffer);
#else
	char buffer[255] = {0};
	getcwd(buffer,255-1);
#endif
	return buffer;
}

bool XLFileUtil::findfile(const std::string & dir,const std::function< bool(const std::string& filename,const std::string& fullfilename) >& callback)
{
	const auto dirname = XLStringUtil::pathseparator(dir);

#if _MSC_VER
	const auto whilddirname = dirname + "\\*.*";

	WIN32_FIND_DATA data;
	HANDLE han = ::FindFirstFile(whilddirname.c_str() , &data);
	if (han == INVALID_HANDLE_VALUE)
	{
		return false;
	}
	do
	{
		if (
			(data.cFileName[0] == '.' && data.cFileName[1] == 0)
				&&
			(data.cFileName[0] == '.' && data.cFileName[1] == '.' && data.cFileName[2] == 0)
			)
		{
			continue;
		}

		if ( ! callback(data.cFileName,dirname + "\\" + data.cFileName) )
		{
			break;
		}
	}
	while( ::FindNextFile(han,&data) );
	FindClose(han);
	return true;

#else
	DIR * han = opendir(dirname.c_str());
	if (!han)
	{
		return false;
	}
	dirent* entry;
	while (entry = readdir(han) ) {
		if (
			(entry->d_name[0] == '.' && entry->d_name[1] == 0)
				&&
			(entry->d_name[0] == '.' && entry->d_name[1] == '.' && entry->d_name[2] == 0)
			)
		{
			continue;
		}

		if ( ! callback(entry->d_name,dirname + "/" + entry->d_name) )
		{
			break;
		}
	}
	closedir(han);
#endif
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
