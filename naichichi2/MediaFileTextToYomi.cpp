#include "common.h"
#include "MediaFileTextToYomi.h"

MediaFileTextToYomi::MediaFileTextToYomi(void)
{
#ifdef _WINDOWS
	this->FELanguage = NULL;
#else
	this->Tagger = NULL;
#endif
}


MediaFileTextToYomi::~MediaFileTextToYomi(void)
{
#ifdef _WINDOWS
	if ( this->FELanguage )
	{
		this->FELanguage->Close();
		this->FELanguage->Release();
	}
#else
#endif

}

xreturn::r<bool> MediaFileTextToYomi::Create()
{
#ifdef _WINDOWS
	HRESULT hr;

	CLSID clsid; //{EB144E8A-AF48-478B-8885-641A0BD2F56A} らしい。
	hr = CLSIDFromString(L"MSIME.Japan", &clsid);
	if(FAILED(hr))	 return xreturn::windowsError(hr);
	
	//hr = this->FELanguage.CoCreateInstance(clsid); //うまく __uuidof がとれない.
	hr = CoCreateInstance(clsid, NULL, CLSCTX_ALL, IID_IFELanguage, (void **)&this->FELanguage);
	if(FAILED(hr))	 return xreturn::windowsError(hr);

	hr = this->FELanguage->Open();
	if(FAILED(hr))	 return xreturn::windowsError(hr);

#else
	const std::string option = std::string("-d ") + dicpath;
	this->Tagger = MeCab::createTagger(option.c_str());
	if (this->Tagger == NULL)
	{
		return xreturn::error("mecab のインスタンスを作れませんでした");
	}
#endif
	return true;
}

xreturn::r<std::string> MediaFileTextToYomi::Parse(const std::string& str)
{
#ifdef _WINDOWS
	_USE_WINDOWS_ENCODING;
	HRESULT hr;
	MORRSLT *pmorrslt;

	auto wstr = _A2W( str.c_str() );
	hr = this->FELanguage->GetJMorphResult(FELANG_REQ_REV, FELANG_CMODE_PINYIN | FELANG_CMODE_NOINVISIBLECHAR, wcslen(wstr), wstr, NULL, &pmorrslt);
	if(FAILED(hr))	 return xreturn::windowsError(hr);

	pmorrslt->pwchOutput[pmorrslt->cchOutput] = 0;
	std::string hiragana = _W2A(pmorrslt->pwchOutput);
	
	::CoTaskMemFree(pmorrslt);

	return hiragana;
#else //_WINDOWS
	const MeCab::Node* node = this->Tagger->parseToNode( str.c_str() );
	if (node == NULL)
	{
		return ;
	}
	std::string hiragana;
	for (; node; node = node->next) 
	{
			if (node->stat == MECAB_BOS_NODE || node->stat == MECAB_EOS_NODE)
			{
				return ;
			}
			if (node->posid <= 9 || node->length <= 0)
			{//記号系はすべて消す.
				return ;
			}
			if (node->surface[0] == '-')
			{
				return;
			}
			std::string word = std::string(node->surface, 0,node->length);
			std::string feature = node->feature;

			auto rcAttr =  node->rcAttr;
			auto posid =  node->posid;
			auto char_type =  node->char_type;
			auto stat =  node->stat;
			auto isbest =  node->isbest;
			auto alpha =  node->alpha;
			auto beta =  node->beta;
			auto prob =  node->prob;
			auto cost =  node->cost;

			return ;
	}
	return hiragana;
#endif
}
