// RSpeechRecognition.cpp: RSpeechRecognition クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RSpeechRecognition.h"
#include "RStdioFile.h"
//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

RSpeechRecognition::RSpeechRecognition()
{

}

RSpeechRecognition::~RSpeechRecognition()
{

}

void RSpeechRecognition::Create(const string & inToWave) throw(RComException)
{
	string dummy = "";
	this->Create(inToWave,dummy);
}

//音声認識のためのオブジェクトの構築.
void RSpeechRecognition::Create(const string & inToWave,const string & inGrammarXML) throw(RComException)
{
	USES_CONVERSION;

	HRESULT hr;

	// 認識エンジンオブジェクトの作成
	//	CLSID_SpSharedRecognizer		共有オブジェクト
	//	CLSID_SpInprocRecognizer		アプリ内動作
	
	if ( inToWave.empty() )
	{
//		hr = this->Engine.CoCreateInstance(CLSID_SpSharedRecognizer);
//		if(FAILED(hr))	throw RComException(hr , "CLSID_SpSharedRecognizer 構築 に失敗");
		hr = this->Engine.CoCreateInstance(CLSID_SpInprocRecognizer);
		if(FAILED(hr))	throw RComException(hr , "CLSID_SpInprocRecognizer 構築 に失敗");


		CComPtr<ISpAudio> cpAudio;
		hr = SpCreateDefaultObjectFromCategoryId(SPCAT_AUDIOIN, &cpAudio);
		if(FAILED(hr))	throw RComException(hr , "SpCreateDefaultObjectFromCategoryId に失敗");

		//認識エンジンのエンジンのディフォルトに設定する。
		hr = this->Engine->SetInput(cpAudio, TRUE);
		if(FAILED(hr))	throw RComException(hr , "SetInput に失敗");

//		hr = this->Engine->SetRecoState( SPRST_ACTIVE );
//		if(FAILED(hr))	throw RComException(hr , "SetRecoState に失敗");
	}
	else
	{
		CComPtr<ISpStream> cpStream;

		hr = this->Engine.CoCreateInstance(CLSID_SpInprocRecognizer);
		if(FAILED(hr))	throw RComException(hr , "CLSID_SpInprocRecognizer 構築 に失敗");

		hr = cpStream.CoCreateInstance(CLSID_SpStream);
		if(FAILED(hr))	throw RComException(hr , "CoCreateInstance  CLSID_SpStream に失敗");

		hr = cpStream->BindToFile( A2W( inToWave.c_str() ) , SPFM_OPEN_READONLY , NULL , NULL,  SPFEI_ALL_EVENTS);  
	    if(FAILED(hr))	throw RComException(hr , "BindToFile に失敗");

		hr = this->Engine->SetInput( cpStream, TRUE);  
	    if(FAILED(hr))	throw RComException( this->Engine , CLSID_SpSharedRecognizer , hr , "SetInput に失敗");
	}

	// 認識コンテクストオブジェクトの作成
	hr = this->Engine->CreateRecoContext(&this->RecoCtxt);
	if(FAILED(hr))	throw RComException(hr , "CreateRecoContext に失敗");

	hr = this->RecoCtxt->SetNotifyWin32Event();
	if ( FAILED(hr) )	throw RComException(hr , "SetNotifyWin32Event に失敗");

	hr = this->RecoCtxt->SetInterest(SPFEI(SPEI_RECOGNITION), SPFEI(SPEI_RECOGNITION));
	if ( FAILED(hr) )	throw RComException(hr , "SetInterest に失敗");

	hr = this->RecoCtxt->SetAudioOptions(SPAO_RETAIN_AUDIO, NULL, NULL);
	if ( FAILED(hr) )	throw RComException(hr , "SetAudioOptions に失敗");


	//メインとなる文法の作成
	hr = this->RecoCtxt->CreateGrammar(0, &this->DictationGrammar);
	if ( FAILED(hr) )	throw RComException(hr , "CreateGrammar に失敗");

	hr = this->DictationGrammar->LoadDictation(NULL, SPLO_STATIC);
	if ( FAILED(hr) )	throw RComException(hr , "LoadDictation に失敗");

	if ( inGrammarXML.empty() )
	{
		//録音開始
		hr = this->DictationGrammar->SetDictationState( SPRS_ACTIVE );
		if ( FAILED(hr) )	throw RComException(hr , "SetDictationState に失敗");
	}
	else
	{
		//ユーザ指定ファイルからのロード
		hr = this->DictationGrammar->LoadCmdFromFile( A2W( inGrammarXML.c_str() ) ,SPLO_STATIC);
		if ( FAILED(hr) )	throw RComException(hr , "LoadCmdFromFile に失敗");

		//録音開始
		hr = this->DictationGrammar->SetRuleState(NULL, NULL, SPRS_ACTIVE );
		if ( FAILED(hr) )	throw RComException(hr , "SetRuleState に失敗");
	}
}

//認識開始
void RSpeechRecognition::Listen() throw(RComException)
{
	USES_CONVERSION;
	HRESULT hr;

	CSpEvent event;

	//録音が終わるまで大待機
	hr = this->RecoCtxt->WaitForNotifyEvent(INFINITE);
	if ( FAILED(hr) )	throw RComException(hr , "WaitForNotifyEvent に失敗");

	hr = event.GetFrom( this->RecoCtxt );
	if ( FAILED(hr) )	throw RComException(hr , "GetFrom に失敗");

	//認識した結果
	ISpRecoResult* result;
	result = event.RecoResult();

	//認識した文字列の取得
	CSpDynamicString dstrText;
	hr = result->GetText(SP_GETWHOLEPHRASE, SP_GETWHOLEPHRASE, TRUE, &dstrText, NULL);
	if ( FAILED(hr) )	throw RComException(hr , "録音したテキストの取得に失敗しました");
	this->ResultString = W2A(dstrText);

	//認識に XMLを使用した場合、代入された結果を得る.
	SPPHRASE *pPhrase;
	event.RecoResult()->GetPhrase(&pPhrase);
	const SPPHRASEPROPERTY *pProp;
	for (pProp = pPhrase->pProperties; pProp; pProp = pProp->pNextSibling)
	{
		string a = W2A(pProp->pszName);
		this->ResultMap[ W2A(pProp->pszName) ] = W2A(pProp->pszValue);
	}
	CoTaskMemFree(pPhrase);

/*
//デバッグのため、読み取った音声をwaveファイルに保存してみる。
	//ファイルに保存. save
	{
		CComPtr<ISpStreamFormat>	ResultStream;

		CComPtr<ISpVoice> voice;
		hr = this->RecoCtxt->GetVoice(&voice);
		if(FAILED(hr))	throw RComException(hr , "GetVoice に失敗");

		hr = event.RecoResult()->GetAudio( 0, 0, &ResultStream );
		if ( FAILED(hr) )	throw RComException(hr , "GetAudio に失敗しました");
		{
			CComPtr<ISpStream> cpWavStream; 
			CComPtr<ISpStreamFormat> cpOldStream; 
			CSpStreamFormat OriginalFmt; 
			voice->GetOutputStream( &cpOldStream ); 
			OriginalFmt.AssignFormat(cpOldStream); 
			hr = SPBindToFile( L"C:\\Users\\rti\\Desktop\\naichichi\\test\\output.wav",SPFM_CREATE_ALWAYS, 
				&cpWavStream,&OriginalFmt.FormatId(), 
				OriginalFmt.WaveFormatExPtr() 	); 
			voice->SetOutput(cpWavStream,TRUE); 
		}
	}
*/
}
