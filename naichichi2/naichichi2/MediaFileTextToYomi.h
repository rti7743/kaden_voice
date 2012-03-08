#pragma once
#ifdef _WINDOWS
//windowsだと COM の IID_IFELanguage  を使います。
//http://www.microsoft.com/download/en/details.aspx?displaylang=en&id=9739 からダウンロードしてきてね
//ダウンロードしたら C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A\Include とかにでもぶち込んで
const IID IID_IFELanguage = { 0x019f7152, 0xe6db, 0x11d0, { 0x83, 0xc3, 0x00, 0xc0, 0x4f, 0xdd, 0xb8, 0x2e }};
#include <msime.h>
#else
//linuxだと mecabを読み抽出だけに使う富豪的環境 -llibmecab でよろ
#include <mecab.h>
#endif

class MediaFileTextToYomi
{
public:
	MediaFileTextToYomi();
	virtual ~MediaFileTextToYomi();
	xreturn::r<bool> Create();
	xreturn::r<std::string> MediaFileTextToYomi::Parse(const std::string& str);
private:
#ifdef _WINDOWS
	IFELanguage* FELanguage;
#else
	MeCab::Tagger *Tagger;
#endif
};
