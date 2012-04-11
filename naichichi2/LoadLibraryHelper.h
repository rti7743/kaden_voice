#pragma once
#include "xreturn.h"

//dll読み込みヘルパー
class LoadLibraryHelper
{
private:
	//DLL インスタンス.
	HMODULE DllInstance;

public:
	LoadLibraryHelper()
	{
		this->DllInstance = NULL;
	}
	virtual ~LoadLibraryHelper()
	{
		if (this->DllInstance != NULL)
		{
			::FreeLibrary(this->DllInstance);
			this->DllInstance = NULL;
		}
	}
	xreturn::r<bool> Load( const std::string& inDLLName )//std読んでいない化石環境とかのために const char* で作る.
	{
		assert(this->DllInstance == NULL);

		this->DllInstance = ::LoadLibraryA(inDLLName.c_str() );
		if ( this->DllInstance == NULL )
		{
			DWORD lastError = ::GetLastError();
			return xreturn::windowsError(std::string() + "ライブラリ" + inDLLName + "を読み込めませんでした",lastError);
		}
		return true;
	}
	FARPROC GetProcAddress(const char* inProcName)
	{
		assert(this->DllInstance != NULL);
		return ::GetProcAddress(this->DllInstance,inProcName);
	}
};
