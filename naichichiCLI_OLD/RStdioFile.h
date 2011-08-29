// RStdioFile.h: RStdioFile クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RSTDIOFILE_H__76E8CC3C_3C3C_4B05_9C45_B3EC2ED04D96__INCLUDED_)
#define AFX_RSTDIOFILE_H__76E8CC3C_3C3C_4B05_9C45_B3EC2ED04D96__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "comm.h"
#include "RIOCannotOpenException.h"
#include "RIOCannotReadException.h"
#include "RIOCannotWriteException.h"

class RStdioFile  
{
public:
	RStdioFile();
	virtual ~RStdioFile();

	//開く.
	void Open(string inFilename,string inMode) throw (RIOCannotOpenException);

	//閉じる.
	void Close();

	//読み込む
	string ReadLine() throw (RIOCannotReadException);

	//書き込む
	void WriteLine(string inWrittingString) throw (RIOCannotWriteException);

	int fread(void* inPtr , int inSize , int Count = 1)
	{
		return ::fread(inPtr , inSize , Count , FilePointer);
	}
	int fwrite(const void* inPtr , int inSize , int Count = 1)
	{
		return ::fwrite(inPtr , inSize , Count , FilePointer);
	}
	int fscan(char* Format,...)
	{
		return ::fscanf( FilePointer , Format,(char*)(&Format+1));
	}

	long fseek(long inP , int inFlg = SEEK_SET)
	{
		return ::fseek(FilePointer , inP , inFlg);
	}

	long ftell() const
	{
		return ::ftell(FilePointer);
	}

	long getSize() 
	{
		long now = ftell();

		fseek(0,SEEK_END);

		long ret = ftell();
		fseek(now);
		return ret;
	}

	//WinCE などで、サスペンドして復帰すると、
	//ファイルハンドルが無効になる現象がある。
	//これを回避するために、もしハンドルが無効になっていたら、
	//指定したファイルを開きなおすことで対処する.
	void KeepHanlde(const string &inFilename , const string & inMode) throw(RIOCannotOpenException)
	{
		if ( ::ftell(FilePointer) == -1L )
		{
			FilePointer = ::fopen(inFilename.c_str() , inMode.c_str() );
			if (!FilePointer)
			{
				throw RIOCannotOpenException(EXCEPTIONTRACE + "ファイル " + inFilename + " が開けません" );
			}
		}
	}

	//おわりっすか?
	bool IsEof();

	FILE*	getHandle() const
	{
		return FilePointer;
	}
private:
	FILE	*FilePointer;

	RBuffer	Buffer;
};

#endif // !defined(AFX_RSTDIOFILE_H__76E8CC3C_3C3C_4B05_9C45_B3EC2ED04D96__INCLUDED_)
