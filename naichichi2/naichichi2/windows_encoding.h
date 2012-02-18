#pragma once
//windows でも UTF8で快適にコーディングできるようにする!!
#include <windows.h>

namespace windows_encoding
{
	static wchar_t* _atou16_convert(const char* inSrc,wchar_t* outBuffer , int inDestSize)
	{
		outBuffer[0] = 0;
		::MultiByteToWideChar(CP_ACP,0,inSrc,-1,outBuffer,inDestSize);

		return outBuffer;
	}
	static char* _u16toa_convert(const wchar_t* inSrc, char* outBuffer , int inDestSize)
	{
		outBuffer[0] = 0;
		::WideCharToMultiByte(CP_ACP,0,inSrc,-1,outBuffer,inDestSize , NULL,NULL);

		return outBuffer;
	}
	static char* _u16tou8_convert(const wchar_t* inSrc, char* outBuffer , int inDestSize)
	{
		outBuffer[0] = 0;
		::WideCharToMultiByte(CP_UTF8,0,inSrc,-1,outBuffer,inDestSize , NULL,NULL);

		return outBuffer;
	}
	static wchar_t* _u8tou16_convert(const char* inSrc,wchar_t* outBuffer , int inDestSize)
	{
		outBuffer[0] = 0;
		::MultiByteToWideChar(CP_UTF8,0,inSrc,-1,outBuffer,inDestSize);

		return outBuffer;
	}

	static char* _atou8_convert(const char * inSrc, char* outBuffer , int inDestSize)
	{
		//ascii -> utf16に変換
		const wchar_t* temp16str = _atou16_convert(inSrc,(wchar_t*)_alloca(inDestSize) , inDestSize);

		//utf16 -> utf8に変換
		return _u16tou8_convert(temp16str , outBuffer,inDestSize);
	}
	static char* _u8toa_convert(const char* inSrc,char* outBuffer , int inDestSize)
	{
		//utf8 -> utf16に変換
		const wchar_t* temp16str = _u8tou16_convert(inSrc,(wchar_t*)_alloca(inDestSize), inDestSize);

		//utf16 -> asciiに変換
		return _u16toa_convert(temp16str , outBuffer,inDestSize);
	}
};
//変換領域のバッファの確保
#define _USE_WINDOWS_ENCODING int _convert;

//ascii -> utf16
#define _A2W(lpa) (\
	(lpa == NULL) ? NULL : ( _convert = (lstrlenA(lpa)+1)*2 , windows_encoding::_atou16_convert(lpa, (wchar_t*)_alloca(_convert),_convert) ) )
//utf16 -> acsii
#define _W2A(lpa) (\
	(lpa == NULL) ? NULL : ( _convert = (lstrlenW(lpa)+1)*2 , windows_encoding::_u16toa_convert(lpa, (char* )_alloca(_convert),_convert) ) )

//ascii -> utf8
#define _A2U(lpa) (\
	(lpa == NULL) ? NULL : ( _convert = (lstrlenA(lpa)+1)*4 , windows_encoding::_atou8_convert(lpa, (char*)_alloca(_convert),_convert) ) )
//utf8 -> ascii
#define _U2A(lpa) (\
	(lpa == NULL) ? NULL : ( _convert = (lstrlenA(lpa)+1)*4 , windows_encoding::_u8toa_convert(lpa, (char*)_alloca(_convert),_convert) ) )

//utf8 -> utf16
#define _U2W(lpa) (\
	(lpa == NULL) ? NULL : ( _convert = (lstrlenA(lpa)+1)*2 , windows_encoding::_u8tou16_convert(lpa, (wchar_t*)_alloca(_convert),_convert) ) )
//utf16 -> utf8
#define _W2U(lpa) (\
	(lpa == NULL) ? NULL : ( _convert = (lstrlenW(lpa)+1)*2 , windows_encoding::_u16tou8_convert(lpa, (char* )_alloca(_convert),_convert) ) )
/*
使い方
void hoge()
{
	const char * sjis = "アスキー"; 	//SJIS
//	UTF8が必要な関数(sjis);     		//UTF8にしたい・・・

	_USE_WINDOWS_ENCODING;				//おまじない
	UTF8が必要な関数(_U2A(sjis));		//UTF8に変換して利用
}


void hoge2()
{
	const char * sjis = "アスキー"; //SJIS

	_USE_WINDOWS_ENCODING;				//おまじない
	UTF8が必要な関数(_A2U(sjis));		//SJIS -> UTF8に変換して利用
	UTF16が必要な関数(_A2W(sjis));		//SJIS -> UTF16変換して利用

	const char * utf8 = "UTF8な文字列";	//UTF8
	SJISが必要な関数(_U2A(sjis));		//UTF8 -> SJISに変換
	UTF16が必要な関数(_U2W(sjis));		//UTF8 -> UTF16変換して利用
}
*/


