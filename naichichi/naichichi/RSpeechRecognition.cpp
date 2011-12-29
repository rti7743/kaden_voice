// RSpeechRecognition.cpp: RSpeechRecognition クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RSpeechRecognition.h"
//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

RSpeechRecognition::RSpeechRecognition()
{

}

RSpeechRecognition::~RSpeechRecognition()
{

}

//音声認識のためのオブジェクトの構築.
void RSpeechRecognition::Create(const std::string & inDicticationFilterWord , const std::string & inGrammarXML , HWND inWindow , UINT inCallbackMesage )
{
	USES_CONVERSION;

	HRESULT hr;

	this->DicticationFilterWord = inDicticationFilterWord;
	this->DictationReady = false;
	this->RuleReady = false;
	this->CallbackWindowHandle = inWindow;
	this->CallbackWindowMesage = inCallbackMesage;

	//Dictation
	{
		CComPtr<ISpAudio> cpAudio;
		hr = SpCreateDefaultObjectFromCategoryId(SPCAT_AUDIOIN, &cpAudio);
		if(FAILED(hr))	 AfxThrowOleException(hr);

		hr = this->DictationEngine.CoCreateInstance(CLSID_SpInprocRecognizer);
		if(FAILED(hr))	 AfxThrowOleException(hr);

		//オーディオから読み込んでね
		hr = this->DictationEngine->SetInput( cpAudio, TRUE);  
		if(FAILED(hr))	 AfxThrowOleException(hr);

		hr = this->DictationEngine->CreateRecoContext(&this->DictationRecoCtxt);
		if(FAILED(hr))	 AfxThrowOleException(hr);

		hr = this->DictationRecoCtxt->SetInterest(SPFEI(SPEI_RECOGNITION), SPFEI(SPEI_RECOGNITION));
		if(FAILED(hr))	 AfxThrowOleException(hr);

		hr = this->DictationRecoCtxt->SetAudioOptions(SPAO_RETAIN_AUDIO, NULL, NULL);
		if(FAILED(hr))	 AfxThrowOleException(hr);

		//認識器始動
		hr = this->DictationRecoCtxt->CreateGrammar(0, &this->DictationGrammar);
		if(FAILED(hr))	 AfxThrowOleException(hr);

		hr = this->DictationGrammar->LoadDictation(NULL, SPLO_STATIC);
		if(FAILED(hr))	 AfxThrowOleException(hr);

		hr = this->DictationRecoCtxt->SetNotifyCallbackFunction(__callbackDictation , (WPARAM)this , 0);
		if(FAILED(hr))	 AfxThrowOleException(hr);

		hr = this->DictationGrammar->SetDictationState(SPRS_ACTIVE );
		if(FAILED(hr))	 AfxThrowOleException(hr);
	}
	//ルールベースのエンジンを作る.
	{
		CComPtr<ISpAudio> cpAudio;
		hr = SpCreateDefaultObjectFromCategoryId(SPCAT_AUDIOIN, &cpAudio);
		if(FAILED(hr))	 AfxThrowOleException(hr);

		hr = this->RuleEngine.CoCreateInstance(CLSID_SpInprocRecognizer);
		if(FAILED(hr))	 AfxThrowOleException(hr);

		//オーディオから読み込んでね
		hr = this->RuleEngine->SetInput( cpAudio, TRUE);  
		if(FAILED(hr))	 AfxThrowOleException(hr);

		hr = this->RuleEngine->CreateRecoContext(&this->RuleRecoCtxt);
		if(FAILED(hr))	 AfxThrowOleException(hr);

		hr = this->RuleRecoCtxt->SetInterest(SPFEI(SPEI_RECOGNITION), SPFEI(SPEI_RECOGNITION));
		if(FAILED(hr))	 AfxThrowOleException(hr);

		hr = this->RuleRecoCtxt->SetAudioOptions(SPAO_RETAIN_AUDIO, NULL, NULL);
		if(FAILED(hr))	 AfxThrowOleException(hr);

		//認識器始動
		hr = this->RuleRecoCtxt->CreateGrammar(0, &this->RuleGrammar);
		if(FAILED(hr))	 AfxThrowOleException(hr);

		hr = this->RuleGrammar->LoadDictation(NULL, SPLO_STATIC);
		if(FAILED(hr))	 AfxThrowOleException(hr);

		hr = this->RuleGrammar->LoadCmdFromFile( A2W( inGrammarXML.c_str() ) ,SPLO_STATIC);
		if(FAILED(hr))	 AfxThrowOleException(hr);

		hr = this->RuleRecoCtxt->SetNotifyCallbackFunction(__callbackRule , (WPARAM)this , 0);
		if(FAILED(hr))	 AfxThrowOleException(hr);

		//録音開始
		hr = this->RuleGrammar->SetRuleState(NULL, NULL, SPRS_ACTIVE );
		if(FAILED(hr))	 AfxThrowOleException(hr);
	}
}

void RSpeechRecognition::CallbackDictation()
{
	USES_CONVERSION;
	HRESULT hr;
	CSpEvent event;

	hr = event.GetFrom( this->DictationRecoCtxt );
	if ( FAILED(hr) )	return ;

	//認識した結果
	ISpRecoResult* result;
	result = event.RecoResult();

	//認識した文字列の取得
	CSpDynamicString dstrText;
	hr = result->GetText(SP_GETWHOLEPHRASE, SP_GETWHOLEPHRASE, TRUE, &dstrText, NULL);
	if ( FAILED(hr) )	return ;
	this->DictationString = W2A(dstrText);

	if ( ! this->RuleReady )
	{
		//自分は完了したが、ルールベースがまだ。自分は完了したフラグを立てる。
		this->DictationReady = true;
	}
	else
	{
		//ディクテーションフィルターで絞る
		this->RuleReady = false;
		if ( this->DictationString.find(this->DicticationFilterWord) == std::string::npos )
		{
			//ディクテーションフィルターで落とす
			return ;
		}
		//コマンド認識
		SendMessage(this->CallbackWindowHandle , this->CallbackWindowMesage , 0 , 0);
	}

}

void RSpeechRecognition::CallbackRule()
{
	USES_CONVERSION;
	HRESULT hr;
	std::string dictationString;

	CSpEvent ruleEvent;
	hr = ruleEvent.GetFrom( this->RuleRecoCtxt );
	if ( FAILED(hr) )	return ;

	//認識した結果
	ISpRecoResult* result;
	result = ruleEvent.RecoResult();

	//認識した文字列の取得
	CSpDynamicString dstrText;
	hr = result->GetText(SP_GETWHOLEPHRASE, SP_GETWHOLEPHRASE, TRUE, &dstrText, NULL);
	if ( FAILED(hr) )	return ;
	this->ResultString = W2A(dstrText);

	//認識に XMLを使用した場合、代入された結果を得る.
	SPPHRASE *pPhrase;
	hr = ruleEvent.RecoResult()->GetPhrase(&pPhrase);
	if ( FAILED(hr) )	return ;

	this->ResultMap.clear();
	const SPPHRASEPROPERTY *pProp;
	for (pProp = pPhrase->pProperties; pProp; pProp = pProp->pNextSibling)
	{
		std::string a = W2A(pProp->pszName);
		this->ResultMap[ W2A(pProp->pszName) ] = W2A(pProp->pszValue);
	}
	CoTaskMemFree(pPhrase);

	if ( ! this->DictationReady )
	{
		//ディクテーションが完了していない。 ルールは完了しているフラグだけを立てる。
		this->RuleReady = true;
	}
	else
	{
		//ディクテーションフィルターで絞る
		this->DictationReady = false;
		if ( this->DictationString.find(this->DicticationFilterWord) == std::string::npos )
		{
			//フィルターにより拒否
			return ;
		}
		//コマンド認識
		SendMessage(this->CallbackWindowHandle , this->CallbackWindowMesage , 0 , 0);
	}
}

