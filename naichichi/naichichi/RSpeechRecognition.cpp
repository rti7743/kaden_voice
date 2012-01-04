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
	this->CallbackWindowHandle = inWindow;
	this->CallbackWindowMesage = inCallbackMesage;

	//Dictation
	{
		CComPtr<ISpAudio> cpAudio;
		hr = SpCreateDefaultObjectFromCategoryId(SPCAT_AUDIOIN, &cpAudio);
		if(FAILED(hr))	 AfxThrowOleException(hr);

		hr = this->DictationEngine.CoCreateInstance(CLSID_SpInprocRecognizer);
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

		hr = this->DictationRecoCtxt->SetNotifyWin32Event();
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
	this->FlagCleanup();
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

	//ルールベースで認識した結果の音声部分をもう一度 ディクテーションにかけます。
	//これで過剰なマッチを排除します。
	{
		CComPtr<ISpStreamFormat>	resultStream;
		hr = result->GetAudio( 0, 0, &resultStream );
		if ( FAILED(hr) )	return;

		//オーディオから読み込んでね
		hr = this->DictationEngine->SetInput( resultStream, TRUE);  
		if(FAILED(hr))	 return;

		hr = this->DictationGrammar->SetDictationState(SPRS_ACTIVE );
		if(FAILED(hr))	 return;

		hr = this->DictationRecoCtxt->WaitForNotifyEvent(10000); //10秒タイムアウト
		if ( FAILED(hr) )	return;

		hr = this->DictationGrammar->SetDictationState(SPRS_INACTIVE );
		if(FAILED(hr))	 return;

		CSpEvent tempevent;
		hr = tempevent.GetFrom( this->DictationRecoCtxt );
		if ( FAILED(hr) )	return ;

		//認識した結果
		ISpRecoResult* tempresult;
		tempresult = tempevent.RecoResult();

		//認識した文字列の取得
		CSpDynamicString tempdstrText;
		hr = tempresult->GetText(SP_GETWHOLEPHRASE, SP_GETWHOLEPHRASE, TRUE, &tempdstrText, NULL);
		if ( FAILED(hr) )	return ;
		std::string dictationString = W2A(tempdstrText);
		//ディクテーションフィルターで絞る
		if ( dictationString.find(this->DicticationFilterWord) == std::string::npos )
		{
			//フィルターにより拒否
			this->FlagCleanup();
			return ;
		}
	}


	//認識に XMLを使用した場合、代入された結果を得る.
	SPPHRASE *pPhrase;
	hr = result->GetPhrase(&pPhrase);
	if ( FAILED(hr) )	return ;

	this->ResultMap.clear();
	const SPPHRASEPROPERTY *pProp;
	for (pProp = pPhrase->pProperties; pProp; pProp = pProp->pNextSibling)
	{
		this->ResultMap[ W2A(pProp->pszName) ] = W2A(pProp->pszValue);
	}
	CoTaskMemFree(pPhrase);

	//コマンド認識
	SendMessage(this->CallbackWindowHandle , this->CallbackWindowMesage , 0 , 0);
	this->FlagCleanup();
}

void RSpeechRecognition::FlagCleanup()
{
	this->RuleReady = false;
	this->ResultMap.clear();
	this->ResultString = "";
}