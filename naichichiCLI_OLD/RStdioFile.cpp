// RStdioFile.cpp: RStdioFile クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "comm.h"
#include "RStdioFile.h"

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

RStdioFile::RStdioFile()
{
	FilePointer = NULL;
}

RStdioFile::~RStdioFile()
{
	Close();
}

//開く.
void RStdioFile::Open(string inFilename,string inMode) throw (RIOCannotOpenException)
{
	Close();

	FilePointer = ::fopen(inFilename.c_str() , inMode.c_str() );
	if (!FilePointer)
	{
		throw RIOCannotOpenException(EXCEPTIONTRACE + "ファイル " + inFilename + " が開けません" );
	}

#ifndef _WIN32_WCE
	Buffer.reserve(32768);
	setvbuf(FilePointer , &Buffer[0] , _IOFBF , 32768);
#endif
}

//閉じる.
void RStdioFile::Close()
{
	if (FilePointer)
	{
		fclose(FilePointer);
		FilePointer = NULL;
	}
}


//読み込む.
string RStdioFile::ReadLine() throw (RIOCannotReadException)
{
	ASSERT(FilePointer != NULL);

	string ret;

	char buf[MAX_PATH * 10];
	while(!feof(FilePointer))
	{
		if (  ::fgets( buf , MAX_PATH * 10, FilePointer) == NULL)
		{
			if (feof(FilePointer))	break;	//正常終了.
			throw RIOCannotReadException("読み込み中にエラーが発生しました");
		}

		int skip = ret.size() ;

		//読み込んだバッファに連結.
		ret += buf;
		//改行がありますか?
		if ( ret.find( string("\r\n") , skip) != -1) break;
	}
	return ret;
}

//書き込む
void RStdioFile::WriteLine(string inWrittingString) throw (RIOCannotWriteException)
{
	ASSERT(FilePointer != NULL);

	int count = ::fwrite( inWrittingString.c_str() , sizeof(char) , inWrittingString.size(), FilePointer);
	if (count != inWrittingString.size())
		throw RIOCannotWriteException("書き込み中にエラーが発生しました");
}

//おわりっすか?
bool RStdioFile::IsEof()
{
	return feof(FilePointer) != 0;
}
