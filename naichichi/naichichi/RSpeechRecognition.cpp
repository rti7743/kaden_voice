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
void RSpeechRecognition::Create(const std::string & inToWave,const std::string & inGrammarXML , HWND inWindow , UINT inCallbackMesage )
{
	USES_CONVERSION;

	HRESULT hr;

	// 認識エンジンオブジェクトの作成
	//	CLSID_SpSharedRecognizer		共有オブジェクト
	//	CLSID_SpInprocRecognizer		アプリ内動作
	
	if ( inToWave.empty() )
	{
//		hr = this->Engine.CoCreateInstance(CLSID_SpSharedRecognizer);
//		if(FAILED(hr))	 RComException(hr , "CLSID_SpSharedRecognizer 構築 に失敗");
		hr = this->Engine.CoCreateInstance(CLSID_SpInprocRecognizer);
		if(FAILED(hr))	 AfxThrowOleException(hr);

		CComPtr<ISpAudio> cpAudio;
		hr = SpCreateDefaultObjectFromCategoryId(SPCAT_AUDIOIN, &cpAudio);
		if(FAILED(hr))	 AfxThrowOleException(hr);

		//認識エンジンのエンジンのディフォルトに設定する。
		hr = this->Engine->SetInput(cpAudio, TRUE);
		if(FAILED(hr))	 AfxThrowOleException(hr);

//		hr = this->Engine->SetRecoState( SPRST_ACTIVE );
//		if(FAILED(hr))	 AfxThrowOleException(hr);
	}
	else
	{
		CComPtr<ISpStream> cpStream;

		hr = this->Engine.CoCreateInstance(CLSID_SpInprocRecognizer);
		if(FAILED(hr))	 AfxThrowOleException(hr);

		hr = cpStream.CoCreateInstance(CLSID_SpStream);
		if(FAILED(hr))	 AfxThrowOleException(hr);

		hr = cpStream->BindToFile( A2W( inToWave.c_str() ) , SPFM_OPEN_READONLY , NULL , NULL,  SPFEI_ALL_EVENTS);  
		if(FAILED(hr))	 AfxThrowOleException(hr);

		hr = this->Engine->SetInput( cpStream, TRUE);  
		if(FAILED(hr))	 AfxThrowOleException(hr);
	}

	// 認識コンテクストオブジェクトの作成
	hr = this->Engine->CreateRecoContext(&this->RecoCtxt);
	if(FAILED(hr))	 AfxThrowOleException(hr);

//	hr = this->RecoCtxt->SetNotifyWin32Event();	//イベントの場合.. こっちの方が好きなんだけどthreadががが...
//	if(FAILED(hr))	 AfxThrowOleException(hr);
	hr = this->RecoCtxt->SetNotifyWindowMessage(inWindow,inCallbackMesage,0,0);	//windowメッセージの場合... 
	if(FAILED(hr))	 AfxThrowOleException(hr);

	hr = this->RecoCtxt->SetInterest(SPFEI(SPEI_RECOGNITION), SPFEI(SPEI_RECOGNITION));
	if(FAILED(hr))	 AfxThrowOleException(hr);

	hr = this->RecoCtxt->SetAudioOptions(SPAO_RETAIN_AUDIO, NULL, NULL);
	if(FAILED(hr))	 AfxThrowOleException(hr);


	//メインとなる文法の作成
	hr = this->RecoCtxt->CreateGrammar(0, &this->DictationGrammar);
	if(FAILED(hr))	 AfxThrowOleException(hr);

	hr = this->DictationGrammar->LoadDictation(NULL, SPLO_STATIC);
	if(FAILED(hr))	 AfxThrowOleException(hr);

	if ( inGrammarXML.empty() )
	{
		//録音開始
		hr = this->DictationGrammar->SetDictationState( SPRS_ACTIVE );
		if(FAILED(hr))	 AfxThrowOleException(hr);
	}
	else
	{
		//ユーザ指定ファイルからのロード
		hr = this->DictationGrammar->LoadCmdFromFile( A2W( inGrammarXML.c_str() ) ,SPLO_STATIC);
		if(FAILED(hr))	 AfxThrowOleException(hr);

		//録音開始
		hr = this->DictationGrammar->SetRuleState(NULL, NULL, SPRS_ACTIVE );
		if(FAILED(hr))	 AfxThrowOleException(hr);
	}
}

//認識開始
void RSpeechRecognition::Listen()
{
	USES_CONVERSION;
	HRESULT hr;

	CSpEvent event;

//ここにきているときは、録音が終了している時だ!
//	//録音が終わるまで大待機
//	hr = this->RecoCtxt->WaitForNotifyEvent(INFINITE);
//	if ( FAILED(hr) )	AfxThrowOleException(hr);

	hr = event.GetFrom( this->RecoCtxt );
	if ( FAILED(hr) )	AfxThrowOleException(hr);

	//認識した結果
	ISpRecoResult* result;
	result = event.RecoResult();

	//認識した文字列の取得
	CSpDynamicString dstrText;
	hr = result->GetText(SP_GETWHOLEPHRASE, SP_GETWHOLEPHRASE, TRUE, &dstrText, NULL);
	if ( FAILED(hr) )	AfxThrowOleException(hr);
	this->ResultString = W2A(dstrText);

	//認識に XMLを使用した場合、代入された結果を得る.
	SPPHRASE *pPhrase;
	event.RecoResult()->GetPhrase(&pPhrase);
	const SPPHRASEPROPERTY *pProp;
	for (pProp = pPhrase->pProperties; pProp; pProp = pProp->pNextSibling)
	{
		std::string a = W2A(pProp->pszName);
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
		if(FAILED(hr))	AfxThrowOleException(hr);

		hr = event.RecoResult()->GetAudio( 0, 0, &ResultStream );
		if ( FAILED(hr) )	AfxThrowOleException(hr);
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
