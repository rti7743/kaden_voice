#pragma once
#ifdef _WINDOWS
//windows でも UTF8で快適にコーディングできるようにする!!

const int _CONVERT_BUFFER_SIZE = 65535;

namespace windows_encoding
{
	static wchar_t* _atou16_convert(const char* inSrc,wchar_t* outBuffer)
	{
		outBuffer[0] = 0;
		::MultiByteToWideChar(CP_ACP,0,inSrc,-1,outBuffer,_CONVERT_BUFFER_SIZE);

		return outBuffer;
	}
	static wchar_t* _atou16_convert(const std::string&  inSrc,char* outBuffer)
	{
		return _atou16_convert(inSrc.c_str() ,outBuffer);
	}

	static char* _u16toa_convert(const wchar_t* inSrc, char* outBuffer)
	{
		outBuffer[0] = 0;
		::WideCharToMultiByte(CP_ACP,0,inSrc,-1,outBuffer,_CONVERT_BUFFER_SIZE , NULL,NULL);

		return outBuffer;
	}
	static char* _u16toa_convert(const std::wstring&  inSrc,char* outBuffer)
	{
		return _u16toa_convert(inSrc.c_str() ,outBuffer);
	}

	static char* _u16tou8_convert(const wchar_t* inSrc, char* outBuffer)
	{
		outBuffer[0] = 0;
		::WideCharToMultiByte(CP_UTF8,0,inSrc,-1,outBuffer,_CONVERT_BUFFER_SIZE , NULL,NULL);

		return outBuffer;
	}
	static char* _u16tou8_convert(const std::wstring&  inSrc,char* outBuffer)
	{
		return _u16tou8_convert(inSrc.c_str() ,outBuffer);
	}

	static wchar_t* _u8tou16_convert(const char* inSrc,wchar_t* outBuffer)
	{
		outBuffer[0] = 0;
		::MultiByteToWideChar(CP_UTF8,0,inSrc,-1,outBuffer,_CONVERT_BUFFER_SIZE);

		return outBuffer;
	}
	static wchar_t* _u8tou16_convert(const std::string&  inSrc,char* outBuffer)
	{
		return _u8tou16_convert(inSrc.c_str() ,outBuffer);
	}

	static char* _atou8_convert(const char * inSrc, char* outBuffer)
	{
		std::vector<char*> _buffer2(65535);

		//ascii -> utf16に変換
		const wchar_t* temp16str = _atou16_convert(inSrc,(wchar_t*)&_buffer2[0] );

		//utf16 -> utf8に変換
		return _u16tou8_convert(temp16str , outBuffer);
	}
	static char* _atou8_convert(const std::string&  inSrc,char* outBuffer)
	{
		return _atou8_convert(inSrc.c_str() ,outBuffer);
	}

	static char* _u8toa_convert(const char* inSrc,char* outBuffer)
	{
		std::vector<char*> _buffer2(65535);

		//utf8 -> utf16に変換
		const wchar_t* temp16str = _u8tou16_convert(inSrc,(wchar_t*)&_buffer2[0]);

		//utf16 -> asciiに変換
		return _u16toa_convert(temp16str , outBuffer);
	}
	static char* _u8toa_convert(const std::string&  inSrc,char* outBuffer)
	{
		return _u8toa_convert(inSrc.c_str() ,outBuffer);
	}

};
//変換領域のバッファの確保
#define _USE_WINDOWS_ENCODING std::vector<char*> _buffer(65535);

//ascii -> utf16
#define _A2W(lpa) windows_encoding::_atou16_convert(lpa, (wchar_t*)&_buffer[0])
//utf16 -> acsii
#define _W2A(lpa) windows_encoding::_u16toa_convert(lpa, (char* )&_buffer[0])

//ascii -> utf8
#define _A2U(lpa) windows_encoding::_atou8_convert(lpa, (char*)&_buffer[0])
//utf8 -> ascii
#define _U2A(lpa) windows_encoding::_u8toa_convert(lpa, (char*)&_buffer[0])

//utf8 -> utf16
#define _U2W(lpa) windows_encoding::_u8tou16_convert(lpa, (wchar_t*)&_buffer[0])
//utf16 -> utf8
#define _W2U(lpa) windows_encoding::_u16tou8_convert(lpa, (char* )&_buffer[0])

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

#else  //_WINDOWS
//変換領域のバッファの確保
#define _USE_WINDOWS_ENCODING 

//ascii -> utf16
#define _A2W(lpa)  lpa
//utf16 -> acsii
#define _W2A(lpa)  lpa

//ascii -> utf8
#define _A2U(lpa)  lpa
//utf8 -> ascii
#define _U2A(lpa)  lpa

//utf8 -> utf16
#define _U2W(lpa)  lpa
//utf16 -> utf8
#define _W2U(lpa)  lpa

#endif //_WINDOWS