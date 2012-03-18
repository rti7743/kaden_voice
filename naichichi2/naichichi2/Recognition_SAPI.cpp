// Recognition_SAPI.cpp: Recognition_SAPI クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "common.h"
#include "ScriptRunner.h"
#include "Recognition_Factory.h"
#include "Recognition_SAPI.h"
#include "MainWindow.h"
//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

Recognition_SAPI::Recognition_SAPI()
{

}

Recognition_SAPI::~Recognition_SAPI()
{

}

//音声認識のためのオブジェクトの構築.
xreturn::r<bool> Recognition_SAPI::Create(MainWindow* poolMainWindow)
{
	_USE_WINDOWS_ENCODING;

	HRESULT hr;
	this->GlobalRuleNodeCount = 1;
	this->TemporaryRuleConfidenceFilter = 0.80;
	this->YobikakeRuleConfidenceFilter = 0.80;
	this->BasicRuleConfidenceFilter = 0.80;
	this->UseDictationFilter = true;
	this->TemporaryRuleCount = 0;
	this->IsNeedUpdateRule = true;
	this->PoolMainWindow = poolMainWindow;

	//Dictation
	{
		hr = this->DictationEngine.CoCreateInstance(CLSID_SpInprocRecognizer);
		if(FAILED(hr))	 return xreturn::windowsError(hr);

		hr = this->DictationEngine->CreateRecoContext(&this->DictationRecoCtxt);
		if(FAILED(hr))	 return xreturn::windowsError(hr);

		ULONGLONG hookevent = SPFEI(SPEI_RECOGNITION)|SPFEI(SPEI_FALSE_RECOGNITION);
		hr = this->DictationRecoCtxt->SetInterest(hookevent, hookevent);
		if(FAILED(hr))	 return xreturn::windowsError(hr);


		hr = this->DictationRecoCtxt->SetAudioOptions(SPAO_RETAIN_AUDIO, NULL, NULL);
		if(FAILED(hr))	 return xreturn::windowsError(hr);

		//認識器始動
		hr = this->DictationRecoCtxt->CreateGrammar(0, &this->DictationGrammar);
		if(FAILED(hr))	 return xreturn::windowsError(hr);

//		hr = this->DictationGrammar->LoadDictation(NULL, SPLO_STATIC);
//		if(FAILED(hr))	 return xreturn::windowsError(hr);

		hr = this->DictationRecoCtxt->SetNotifyWin32Event();
		if(FAILED(hr))	 return xreturn::windowsError(hr);

		hr = this->DictationGrammar->GetRule(_A2W("FilterRule") ,0,SRATopLevel | SRADynamic | SPRAF_Active, TRUE ,  &this->FilterRuleHandleHandle); 
		if(FAILED(hr))	 return xreturn::windowsError(hr);

//		hr = this->DictationGrammar->GetRule(_A2W("FilterRule2") ,0,SRATopLevel | SRADynamic | SPRAF_Active, TRUE ,  &this->FilterRuleHandleHandle2); 
//		if(FAILED(hr))	 return xreturn::windowsError(hr);


//		hr = this->DictationGrammar->Commit(0);
//		if(FAILED(hr))	 return xreturn::windowsError(hr);
	}
	//ルールベースのエンジンを作る.
	{
		CComPtr<ISpAudio> cpAudio;
		hr = SpCreateDefaultObjectFromCategoryId(SPCAT_AUDIOIN, &cpAudio);
		if(FAILED(hr))	 return xreturn::windowsError(hr);

		hr = this->RuleEngine.CoCreateInstance(CLSID_SpInprocRecognizer);
		if(FAILED(hr))	 return xreturn::windowsError(hr);

		//オーディオから読み込んでね
		hr = this->RuleEngine->SetInput( cpAudio, TRUE);  
		if(FAILED(hr))	 return xreturn::windowsError(hr);

		hr = this->RuleEngine->CreateRecoContext(&this->RuleRecoCtxt);
		if(FAILED(hr))	 return xreturn::windowsError(hr);

//		ULONGLONG hookevent = SPFEI(SPEI_RECOGNITION)|SPFEI(SPEI_FALSE_RECOGNITION)|SPFEI(SPEI_HYPOTHESIS)| SPFEI(SPEI_SOUND_START) | SPFEI(SPEI_SOUND_END);
		ULONGLONG hookevent = SPFEI(SPEI_RECOGNITION);
		hr = this->RuleRecoCtxt->SetInterest(hookevent, hookevent);
		if(FAILED(hr))	 return xreturn::windowsError(hr);

		hr = this->RuleRecoCtxt->SetAudioOptions(SPAO_RETAIN_AUDIO, NULL, NULL);
		if(FAILED(hr))	 return xreturn::windowsError(hr);

		//認識器始動
		hr = this->RuleRecoCtxt->CreateGrammar(0, &this->RuleGrammar);
		if(FAILED(hr))	 return xreturn::windowsError(hr);
	}
	//トップレベルルールたちを作成する。
//	hr = this->RuleGrammar->GetRule(_A2W("BasicRule") ,0,SRATopLevel | SRADynamic | SPRAF_Active, TRUE ,  &this->BasicRuleHandleHandle); 
//	if(FAILED(hr))	 return xreturn::windowsError(hr);

	hr = this->RuleGrammar->GetRule(_A2W("TemporaryRule") ,0,SRATopLevel | SRADynamic |  SPRAF_Active , TRUE ,  &this->TemporaryRuleHandle); 
	if(FAILED(hr))	 return xreturn::windowsError(hr);

	hr = this->RuleGrammar->GetRule(_A2W("YobikakeRule") ,0,SRATopLevel | SRADynamic |  SPRAF_Active , TRUE ,  &this->YobikakeRuleHandle); 
	if(FAILED(hr))	 return xreturn::windowsError(hr);

	hr = this->RuleGrammar->GetRule(_A2W("CommandRule") ,0,SRADynamic , TRUE ,  &this->CommandRuleHandle); 
	if(FAILED(hr))	 return xreturn::windowsError(hr);
/*
	hr = this->RuleGrammar->GetRule(_A2W("MusicRule") ,0,SRADynamic , TRUE ,  &this->MusicRuleHandle); 
	if(FAILED(hr))	 return xreturn::windowsError(hr);

	hr = this->RuleGrammar->GetRule(_A2W("VideoRule") ,0,SRADynamic , TRUE ,  &this->VideoRuleHandle); 
	if(FAILED(hr))	 return xreturn::windowsError(hr);

	hr = this->RuleGrammar->GetRule(_A2W("BookRule") ,0,SRADynamic , TRUE ,  &this->BookRuleHandle); 
	if(FAILED(hr))	 return xreturn::windowsError(hr);
*/
	//http://msdn.microsoft.com/en-us/library/ee125671(v=vs.85).aspx
	//BasicRule = YobikakeRule + CommandRule
//	SPSTATEHANDLE appendState; //あとに続くを表現するためには、 新規にステートを作らないといけない。
//	hr = this->RuleGrammar->Z(this->BasicRuleHandleHandle, &appendState);
//	if(FAILED(hr))	 return xreturn::windowsError(hr);
//	hr = this->RuleGrammar->AddRuleTransition(this->BasicRuleHandleHandle , appendState , this->YobikakeRuleHandle , 1.0f , NULL );
//	if(FAILED(hr))	 return xreturn::windowsError(hr);
//	hr = this->RuleGrammar->AddRuleTransition(appendState , NULL , this->CommandRuleHandle , 1.0f , NULL );
//	if(FAILED(hr))	 return xreturn::windowsError(hr);

	//////
//	SPSTATEHANDLE appendState; //あとに続くを表現するためには、 新規にステートを作らないといけない。
//	hr = this->RuleGrammar->CreateNewState(this->YobikakeRuleHandle, &appendState);
//	if(FAILED(hr))	 return xreturn::windowsError(hr);
//	hr = this->RuleGrammar->AddRuleTransition(this->YobikakeRuleHandle , appendState , this->CommandRuleHandle , 1.0f , NULL );
//	if(FAILED(hr))	 return xreturn::windowsError(hr);
//	hr = this->RuleGrammar->AddRuleTransition(appendState , NULL , this->CommandRuleHandle , 1.0f , NULL );
//	if(FAILED(hr))	 return xreturn::windowsError(hr);


	//戻りはコールバックで。
	//SAPIのコールバックはメインスレッドにコールバックされる。
	hr = this->RuleRecoCtxt->SetNotifyCallbackFunction
		( methodcallback::registstdcall<struct _sapi_callback1,SPNOTIFYCALLBACK*>(this, &Recognition_SAPI::Callback)  ,0,0);
	if(FAILED(hr))	 return xreturn::windowsError(hr);

	//ルールの適応
//	this->CommitRule();

	return true;
}

void Recognition_SAPI::Callback(WPARAM wParam, LPARAM lParam)
{
	try
	{
		this->PoolMainWindow->SyncInvokeLog("Recognition_SAPI::Callback" ,LOG_LEVEL_DEBUG);
		this->CallbackReco();
	}
	catch(xreturn::error &e)
	{
		this->PoolMainWindow->SyncInvokeError( e.getFullErrorMessage() );
	}
	this->PoolMainWindow->SyncInvokeLog("reco idle" ,LOG_LEVEL_DEBUG);
}


//マッチしたphraseからデータを抽出する.
//複雑になるのでクラス内クラスとして独立させる.
//phrase を開放までをサポートします.
class PhraseTo
{
private:
	//名前が付いているルールをすべてピックアップする.
	class spphraseRuleTracker
	{
	public:
		//ツリー構造になっているのでトップノードから再起的に下りていきます。
		std::list<const SPPHRASERULE*> pickupRules;
		void track(const SPPHRASERULE *rule)
		{
			if (!rule)	return ;
			if (rule->pFirstChild)  track(rule->pFirstChild);
			if (rule->pNextSibling) track(rule->pNextSibling);

			if (!rule->pszName)	return ;
			this->pickupRules.push_back(rule);

		}
	} Track;

	SPPHRASE* Phrase;
public:
	PhraseTo(SPPHRASE* phrase)
	{
		assert(phrase != NULL);
		this->Phrase = phrase;
		if (this->IsError())
		{
			return ;
		}


		if (this->IsTemporaryRule())
		{
			//最初のTemporaryRuleノード読み取る.
			this->Track.track( &this->Phrase->Rule );
		}
		else
		{
			//CommandRule の下を読み取る.
//			this->Track.track(this->Phrase->Rule.pFirstChild->pNextSibling);
			this->Track.track(this->Phrase->Rule.pFirstChild);
		}
	}
	~PhraseTo()
	{
		if (this->Phrase)
		{
			CoTaskMemFree(this->Phrase);
			this->Phrase = NULL;
		}
	}
	//マッチした結果を正規表現キャプチャしたものだけを取得する.
	std::map<std::string , std::string> GetRegexpCapture() const
	{
		_USE_WINDOWS_ENCODING;

		std::map<std::string , std::string> ret;
		for(auto it = this->Track.pickupRules.begin(); it != this->Track.pickupRules.end() ; ++it)
		{
			const SPPHRASERULE* rule = *it;
			const int captureNumber = _wtoi(rule->pszName);

			//キャプチャされた値が入っているところまでスキップ
			unsigned int count = 0;
			const SPPHRASEELEMENT * pElem = this->Phrase->pElements;
			for (; count < rule->ulFirstElement ; ++pElem , count ++) 
				;
			//そこから指定された数の文字列を結合したものがキャプチャした結果になる。
			std::string str;
			for (count = 0; count < rule->ulCountOfElements ; ++pElem , count ++) 
			{
				str = str + _W2A(pElem->pszLexicalForm);
			}

			ret[num2str(captureNumber)] = str;
		}
		return ret;
	}
	std::string GetAllString() const
	{
		_USE_WINDOWS_ENCODING;

		std::string str;
		const SPPHRASEELEMENT * pElem = this->Phrase->pElements;
		const int allcount = this->Phrase->Rule.ulCountOfElements;
		for(int count = 0;count < allcount;++pElem,++count)
		{
			str = str + _W2A(pElem->pszLexicalForm);
		}
		return str;
	}
	double GetYobikakeEngineConfidence() const
	{
		if (this->IsTemporaryRule())
		{
			return 0;
		}
		//呼びかけの部分の信頼度を取得する.
		return this->Phrase->pElements->SREngineConfidence;
	}
	//平均認識率
	double GetSREngineConfidenceAvg() const
	{
		double SREngineConfidenceSum = 0;
		for(auto it = this->Track.pickupRules.begin(); it != this->Track.pickupRules.end() ; ++it)
		{
			const SPPHRASERULE* rule = *it;
			const int captureNumber = _wtoi(rule->pszName);

			//キャプチャされた値が入っているところまでスキップ
			unsigned int count = 0;
			const SPPHRASEELEMENT * pElem = this->Phrase->pElements;
			for (; count < rule->ulFirstElement ; ++pElem , count ++) 
				;
			SREngineConfidenceSum += rule->SREngineConfidence;
		}
		return SREngineConfidenceSum / this->Track.pickupRules.size();
	}
	//コールバックする関数IDの取得
	unsigned int GetFuncID() const
	{
		//最初にNULLじゃないプロパティがでてくるらしい
		const SPPHRASEPROPERTY * pProp = this->Phrase->pProperties;
		if (pProp != NULL && pProp->vValue.vt == VT_UINT)
		{
			return pProp->vValue.ulVal;
		}
		return UINT_MAX;
	}

	bool IsTemporaryRule() const
	{
		return (this->Phrase->Rule.pszName[0] == L'T');
	}
	bool IsError() const
	{
		if (!this->Phrase->Rule.pszName) return true;
//		if (!this->Phrase->Rule.pFirstChild) return true;
		return false;
	}
};

//認識したときに呼ばれるコールバック
xreturn::r<bool> Recognition_SAPI::CallbackReco()
{
	HRESULT hr;
	//平均認識率
	double SREngineConfidenceAvg;
	//正規表現キャプチャ
	std::map<std::string , std::string> capture;
	//呼びかけの部分の信頼度を取得する.
	double yobikakeEngineConfidence;
	//コールバックIDの取得
	unsigned int funcID;
	//テンポラリルールかどうか。
	bool isTemporaryRule;
	//ルールでマッチしたものをディクテーション認識させた時の結果
	std::string dictationString;
	//マッチした文字列全体
	std::string matchString;

	//マッチした結果を取得し分析します。
	{
		CSpEvent ruleEvent;
		hr = ruleEvent.GetFrom( this->RuleRecoCtxt );
		if ( FAILED(hr) )	return xreturn::windowsError(hr);

		if ( ruleEvent.eEventId != SPEI_RECOGNITION )
		{
			return false;
		}
		this->PoolMainWindow->SyncInvokeLog("SPEI_RECOGNITION" ,LOG_LEVEL_DEBUG);

		{
			//認識した結果
			ISpRecoResult* result;
			result = ruleEvent.RecoResult();

			SPPHRASE *pPhrase;
			hr = result->GetPhrase(&pPhrase);
			if ( FAILED(hr) )	return xreturn::windowsError(hr);

			PhraseTo phraseTo(pPhrase);
			if (phraseTo.IsError())
			{
				this->PoolMainWindow->AsyncInvoke( [=](){
					this->PoolMainWindow->ScriptManager.BadVoiceRecogntion(-5,"","",0,0,false); 
				} );
				return false;
			}

			//平均認識率
			SREngineConfidenceAvg = phraseTo.GetSREngineConfidenceAvg();
			//正規表現キャプチャ
			capture = phraseTo.GetRegexpCapture();
			//呼びかけの部分の信頼度を取得する.
			yobikakeEngineConfidence = phraseTo.GetYobikakeEngineConfidence();
			//コールバックIDの取得
			funcID = phraseTo.GetFuncID();
			//テンポラリルール？
			isTemporaryRule = phraseTo.IsTemporaryRule();
			//マッチした文字列
			matchString = phraseTo.GetAllString();
			if ( !isTemporaryRule )
			{//ルールでマッチしたものをディクテーション認識させてみる。
//				dictationString = this->convertDictation(result,"FilterRule");
//				if ( ! this->checkDictation(dictationString) )
//				{
//					dictationString = this->convertDictation(result,"FilterRule2");
//					if ( ! this->checkDictation(dictationString) )
//					{
						dictationString = this->convertDictation(result,"");
//					}
//				}
			}
		}
	}

	if ( funcID == UINT_MAX || funcID >= this->CallbackDictionary.size()  )
	{//コールバックしようがないマッチは異常。
		return xreturn::error("マッチした後のコールバック関数ID " + num2str(funcID) + " が存在しません" );
	}


	if ( isTemporaryRule )
	{//テンポラリルール
		if (SREngineConfidenceAvg < this->TemporaryRuleConfidenceFilter)
		{//BAD
			this->PoolMainWindow->AsyncInvoke( [=](){
				this->PoolMainWindow->ScriptManager.BadVoiceRecogntion
					(-1,matchString,"",0,SREngineConfidenceAvg,false);
			} );
			return false;
		}
		//上手くマッチしたらのでコールバックする
		this->PoolMainWindow->SyncInvokePopupMessage("音声認識",matchString);
		this->PoolMainWindow->AsyncInvoke( [=](){
			this->PoolMainWindow->ScriptManager.VoiceRecogntion
				(this->CallbackDictionary[funcID],capture,"",0,SREngineConfidenceAvg);
		} );
		return true;
	}
	
	//ディクテーションチェック
	bool dictationCheck = this->checkDictation(dictationString);
	if (this->UseDictationFilter)
	{
		if (! dictationCheck )
		{//ディクテーションチェックの結果エラーになった
			this->PoolMainWindow->AsyncInvoke( [=](){
				this->PoolMainWindow->ScriptManager.BadVoiceRecogntion
					(-2,matchString,dictationString,yobikakeEngineConfidence,SREngineConfidenceAvg,dictationCheck);
			} );
			return false;
		}
	}

	//呼びかけの部分の信頼度
	if (yobikakeEngineConfidence <  this->YobikakeRuleConfidenceFilter )
	{//呼びかけの信頼度が足りない
		this->PoolMainWindow->AsyncInvoke( [=](){
			this->PoolMainWindow->ScriptManager.BadVoiceRecogntion
				(-3,matchString,dictationString,yobikakeEngineConfidence,SREngineConfidenceAvg,dictationCheck);
		} );
		return false;
	}

	//全体を通しての信頼度
	if (SREngineConfidenceAvg <  this->BasicRuleConfidenceFilter )
	{//全体を通しての信頼度が足りない
		this->PoolMainWindow->AsyncInvoke( [=](){
			this->PoolMainWindow->ScriptManager.BadVoiceRecogntion
				(-4,matchString,dictationString,yobikakeEngineConfidence,SREngineConfidenceAvg,dictationCheck);
		} );
		return false;
	}

	//マッチしたのでコールバックする
	this->PoolMainWindow->SyncInvokePopupMessage("音声認識",matchString);
	this->PoolMainWindow->AsyncInvoke( [=](){
		this->PoolMainWindow->ScriptManager.VoiceRecogntion
			(this->CallbackDictionary[funcID],capture,dictationString,yobikakeEngineConfidence,SREngineConfidenceAvg);
	} );
	return true;
}

//ディクテーションフィルターで呼びかけ文字列が入っているか確認する.
bool Recognition_SAPI::checkDictation(const std::string & dictationString) const
{
	auto it = this->YobikakeListArray.begin();
	for( ; this->YobikakeListArray.end() != it ; ++it)
	{
		//ディクテーションフィルターで絞る
		if ( dictationString.find( *it ) != std::string::npos )
		{
			break;
		}
	}
	return (this->YobikakeListArray.end() != it);
}

//ルールベースで認識した結果の音声部分をもう一度 ディクテーションにかけます。
//これで過剰なマッチを排除します。
xreturn::r<std::string> Recognition_SAPI::convertDictation(ISpRecoResult* result,const std::string& ruleName)
{
	HRESULT hr;
	_USE_WINDOWS_ENCODING;

	CComPtr<ISpStreamFormat>	resultStream;
	{
		hr = result->GetAudio( 0, 1, &resultStream );
		if(FAILED(hr))	 return xreturn::windowsError(hr);

		//オーディオから読み込んでね
		hr = this->DictationEngine->SetInput( resultStream, TRUE);  
		if(FAILED(hr))	 return xreturn::windowsError(hr);

		hr = this->DictationGrammar->SetRuleState(ruleName.empty() ? NULL : _A2W(ruleName.c_str()), NULL, SPRS_ACTIVE );
		if(FAILED(hr))	 return xreturn::windowsError(hr);

		hr = this->DictationRecoCtxt->WaitForNotifyEvent(2000); //2秒タイムアウト
		if(FAILED(hr))	 return xreturn::windowsError(hr);

		hr = this->DictationGrammar->SetRuleState(NULL, NULL, SPRS_INACTIVE );
		if(FAILED(hr))	 return xreturn::windowsError(hr);

		{
			CSpEvent tempevent;
			hr = tempevent.GetFrom( this->DictationRecoCtxt );
			if(FAILED(hr))	 return xreturn::windowsError(hr);

			if (tempevent.eEventId == SPEI_RECOGNITION)
			{//認識した結果
				ISpRecoResult* tempresult;
				{
					tempresult = tempevent.RecoResult();

					//認識した文字列の取得
					CSpDynamicString tempdstrText;
					hr = tempresult->GetText(SP_GETWHOLEPHRASE, SP_GETWHOLEPHRASE, TRUE, &tempdstrText, NULL);
					if(FAILED(hr))	 return xreturn::windowsError(hr);

					SPPHRASE *pPhrase;
					hr = tempresult->GetPhrase(&pPhrase);
					if ( FAILED(hr) )	return xreturn::windowsError(hr);

					double confidence = pPhrase->pElements->SREngineConfidence;

					std::string ret = _W2A(tempdstrText);
					this->PoolMainWindow->SyncInvokeLog(std::string() + "ディクテーションフィルター :" + ret + + " " + num2str(confidence),LOG_LEVEL_DEBUG);

					if (confidence <= 0.60)
					{
						this->PoolMainWindow->SyncInvokeLog(std::string() + "ディクテーションフィルター棄却",LOG_LEVEL_DEBUG);
						return "";
					}

					return ret;
				}
			}
		}
	}

	//不明
	return "";
}

//デバッグ用 認識結果をWaveファイルとして保存する
xreturn::r<bool> Recognition_SAPI::DebugSaveWavFile(const std::string& directory,ISpStreamFormat* streamFormat) const
{
	HRESULT hr;
	_USE_WINDOWS_ENCODING;

	const SPSTREAMFORMAT spFormat = SPSF_22kHz8BitMono;
	CSpStreamFormat Fmt( spFormat, &hr);
	if(FAILED(hr))	 return xreturn::windowsError(hr);

	{
		CSpStreamFormat OriginalFmt;
		{
			OriginalFmt.AssignFormat(streamFormat);

			// basic SAPI-stream for file-based storage
			CComPtr<ISpStream> cpStream;
			{
				ULONG cbWritten = 0;

				// create file on hard-disk for storing recognized audio, and specify audio format as the retained audio format
				std::string fff = directory + "\\" + num2str(time(NULL))+".wav";
				hr = SPBindToFile(_A2W(fff.c_str()) , SPFM_CREATE_ALWAYS, &cpStream, &OriginalFmt.FormatId(), OriginalFmt.WaveFormatExPtr(), SPFEI_ALL_EVENTS);
				if(FAILED(hr))	 return xreturn::windowsError(hr);

				// Continuously transfer data between the two streams until no more data is found (i.e. end of stream)
				// Note only transfer 1000 bytes at a time to creating large chunks of data at one time
				while (TRUE)
				{
					// for logging purposes, the app can retrieve the recognized audio stream length in bytes
					STATSTG stats;
					hr = streamFormat->Stat(&stats, NULL);
					if(FAILED(hr))	 return xreturn::windowsError(hr);

					// create a 1000-byte buffer for transferring
					BYTE bBuffer[1000];
					ULONG cbRead;

					// request 1000 bytes of data from the input stream
					hr = streamFormat->Read(bBuffer, 1000, &cbRead);
					// if data was returned??
					if (SUCCEEDED(hr) && cbRead > 0)
					{
						// then transfer/write the audio to the file-based stream
						hr = cpStream->Write(bBuffer, cbRead, &cbWritten);
						if(FAILED(hr))	 return xreturn::windowsError(hr);
					}

					// since there is no more data being added to the input stream, if the read request returned less than expected, the end of stream was reached, so break data transfer loop
					if (cbRead < 1000)
					{
						break;
					}
				}
			}
			// explicitly close the file-based stream to flush file data and allow app to immediately use the file
			hr = cpStream->Close();
			if(FAILED(hr))	 return xreturn::windowsError(hr);
		}
	}
	return true;
}

//呼びかけを設定します。
//設定したあと、 CommitRule() てしてね。
xreturn::r<bool> Recognition_SAPI::SetYobikake(const std::list<std::string> & yobikakeList)
{
	_USE_WINDOWS_ENCODING;
	HRESULT hr;

	this->RuleGrammar->ClearRule(this->YobikakeRuleHandle);
	this->DictationGrammar->ClearRule(this->FilterRuleHandleHandle);
//	this->DictationGrammar->ClearRule(this->FilterRuleHandleHandle2);


	SPSTATEHANDLE appendState; //あとに続くを表現するためには、 新規にステートを作らないといけない。
	hr = this->RuleGrammar->CreateNewState(this->YobikakeRuleHandle, &appendState);
	if(FAILED(hr))	 return xreturn::windowsError(hr);
	hr = this->RuleGrammar->AddRuleTransition(appendState , NULL, this->CommandRuleHandle , 1.0f , NULL );
	if(FAILED(hr))	 return xreturn::windowsError(hr);


	this->YobikakeListArray = yobikakeList;
	for(auto it = this->YobikakeListArray.begin();  this->YobikakeListArray.end() != it ; ++it)
	{
		//ふつー使う呼びかけ
		hr = this->RuleGrammar->AddWordTransition(this->YobikakeRuleHandle , appendState , _A2W( it->c_str() ) , L" " , SPWT_LEXICAL , 1.0f , NULL );
		if(FAILED(hr))	 return xreturn::windowsError(hr);

		//ディクテーションフィルターの呼びかけ
//		hr = this->DictationGrammar->AddWordTransition(this->FilterRuleHandleHandle , NULL , _A2W( it->c_str() ) , L" " , SPWT_LEXICAL , 1.0f , NULL );
		hr = this->DictationGrammar->AddWordTransition(this->FilterRuleHandleHandle , NULL , _A2W( it->c_str() ) , L" " , SPWT_LEXICAL , 1.0f , NULL );
		if(FAILED(hr))	 return xreturn::windowsError(hr);
//		hr = this->DictationGrammar->AddWordTransition(this->FilterRuleHandleHandle2 , NULL , _A2W( it->c_str() ) , L" " , SPWT_LEXICAL , 2.0f , NULL );
//		if(FAILED(hr))	 return xreturn::windowsError(hr);
	}
	//ディクテーションフィルター その1はディクテーションノードを加える
//	hr = this->DictationGrammar->AddRuleTransition(this->FilterRuleHandleHandle, NULL, SPRULETRANS_DICTATION, 1.0f, NULL);
//	if(FAILED(hr))	 return xreturn::windowsError(hr);
	//ディクテーションフィルター その2はあ-んを加える.
	{
		wchar_t word[2]; word[0] = L'あ'; word[1] = 0;
		for(; word[0] < L'ん' ; word[0] ++ )
		{
			hr = this->DictationGrammar->AddWordTransition(this->FilterRuleHandleHandle , NULL , word , L" " , SPWT_LEXICAL , 1.0f , NULL );
			if(FAILED(hr))	 return xreturn::windowsError(hr);
		}
	}

	//ルールに変更が加わったのでコミットしないといけません。
	this->IsNeedUpdateRule = true;

	return true;
}

//認識結果で不感染なものを捨てる基準値を設定します。
xreturn::r<bool> Recognition_SAPI::SetRecognitionFilter(double temporaryRuleConfidenceFilter,double yobikakeRuleConfidenceFilter,double basicRuleConfidenceFilter,bool useDictationFilter)
{
	this->TemporaryRuleConfidenceFilter = temporaryRuleConfidenceFilter;
	this->YobikakeRuleConfidenceFilter = yobikakeRuleConfidenceFilter;
	this->BasicRuleConfidenceFilter = basicRuleConfidenceFilter;
	this->UseDictationFilter = useDictationFilter;
	
	return true;
}

//コマンドに反応する音声認識ルールを構築します
xreturn::r<bool> Recognition_SAPI::AddCommandRegexp(const CallbackDataStruct * callback,const std::string & str)
{
	this->CallbackDictionary.push_back(callback);
	assert(this->CallbackDictionary.size() >= 1);
	return AddRegexp(CallbackDictionary.size() - 1,str , this->CommandRuleHandle);
}

//テンポラリルールに反応する音声認識ルールを構築します
xreturn::r<bool> Recognition_SAPI::AddTemporaryRegexp(const CallbackDataStruct * callback,const std::string & str)
{
	this->CallbackDictionary.push_back(callback);
	assert(this->CallbackDictionary.size() >= 1);

	this->TemporaryRuleCount ++;
	return AddRegexp(CallbackDictionary.size() - 1,str, this->TemporaryRuleHandle);
}

//テンポラリルールをすべてクリアします
xreturn::r<bool> Recognition_SAPI::ClearTemporary()
{
	if (this->TemporaryRuleCount <= 0)
	{//現在テンポラリルールにルールが入っていないので、クリアをスキップします。
		this->PoolMainWindow->SyncInvokeLog("音声認識ルールでテンポラリクリアが呼ばましたが、値が空なので無視します。",LOG_LEVEL_DEBUG);
		return true;
	}

	HRESULT hr;

	hr = this->RuleGrammar->ClearRule(this->TemporaryRuleHandle);
	if(FAILED(hr))	 return xreturn::windowsError(hr);

	//クリアしたので、テンポラリルールの数はゼロになります。
	this->TemporaryRuleCount = 0;
	//ルールに変更が加わったのでコミットしないといけません。
	this->IsNeedUpdateRule = true;

	//コミット発動
	auto r = this->CommitRule();
	if (!r) return xreturn::error(r.getError());

	this->PoolMainWindow->SyncInvokeLog("音声認識ルールでテンポラリクリアを実行します",LOG_LEVEL_DEBUG);
	return true;
}

//構築したルールを音声認識エンジンにコミットします。
xreturn::r<bool> Recognition_SAPI::CommitRule()
{
	if (! this->IsNeedUpdateRule )
	{//アップデートする必要なし
		this->PoolMainWindow->SyncInvokeLog("音声認識ルールコミットが呼ばれましたが変更がないので無視します。",LOG_LEVEL_DEBUG);
		return true;
	}

	_USE_WINDOWS_ENCODING;
	HRESULT hr;

	hr = this->RuleGrammar->SetRuleState(NULL, NULL, SPRS_INACTIVE );
	if(FAILED(hr))	 return xreturn::windowsError(hr);

	//通常ルールのコミット
	hr = this->RuleGrammar->Commit(0);
	if(FAILED(hr))	 return xreturn::windowsError(hr);

	//ディクテーションフィルターのコミット
	hr = this->DictationGrammar->Commit(0);
	if(FAILED(hr))	 return xreturn::windowsError(hr);

///これを入れるとキューが詰まる時があるような。(経験則)
///	hr = this->RuleEngine->SetRecoState(SPRST_ACTIVE);
///	if(FAILED(hr))	 return xreturn::windowsError(hr);

	if (this->TemporaryRuleCount >= 1)
	{
		hr = this->RuleGrammar->SetRuleState(NULL, NULL, SPRS_ACTIVE );
		if(FAILED(hr))	 return xreturn::windowsError(hr);

		this->PoolMainWindow->SyncInvokeLog("音声認識ルールコミット:テンポラリルールも含めて commit します",LOG_LEVEL_DEBUG);
	}
	else
	{
//		hr = this->RuleGrammar->SetRuleState(_A2W("BasicRule"),NULL,SPRS_ACTIVE);
		hr = this->RuleGrammar->SetRuleState(_A2W("YobikakeRule"),NULL,SPRS_ACTIVE);
		if(FAILED(hr))	 return xreturn::windowsError(hr);

		this->PoolMainWindow->SyncInvokeLog("音声認識ルールコミット:YobikakeRule以下を commit します",LOG_LEVEL_DEBUG);
	}

	//アップデートが終わったので再びルールに変更が加わるまではアップデートしない。
	this->IsNeedUpdateRule = false;

	return true;
}

//メディア情報をアップデートします。
xreturn::r<bool> Recognition_SAPI::UpdateMedia(const std::string& name ,const std::list<std::string>& list )
{
return true;
/*
	_USE_WINDOWS_ENCODING;
	HRESULT hr;

	SPSTATEHANDLE targetHandle = NULL;
	if (name == "music")
	{
		targetHandle = this->MusicRuleHandle;
	}
	else if (name == "music")
	{
		targetHandle = this->VideoRuleHandle;
	}
	else if (name == "book")
	{
		targetHandle = this->BookRuleHandle;
	}
	else
	{
		return xreturn::error("未定義の" + name + "が選択されました。");
	}

	//単語を追加していきます。
	for(auto it = list.begin() ; it != list.end() ; ++it )
	{
		hr = this->RuleGrammar->AddWordTransition(targetHandle , NULL , _A2W( it->c_str() ) , L" " , SPWT_LEXICAL , 1.0f , NULL );
		if(FAILED(hr))	 return xreturn::windowsError(hr);
	}

	//変更したので次回アップデートしてね。
	this->IsNeedUpdateRule = true;
	return true;
*/
}


//このコールバックに関連付けられているものをすべて消す
xreturn::r<bool> Recognition_SAPI::RemoveCallback(const CallbackDataStruct* callback , bool is_unrefCallback)
{
	return true;
}


//音声認識ルールを構築します。 正規表現にも対応しています。
xreturn::r<bool> Recognition_SAPI::AddRegexp(unsigned int id,const std::string & str ,SPSTATEHANDLE stateHandle ) 
{
	_USE_WINDOWS_ENCODING;

	//ルールに変更が加わるのでアップデートするフラグを立てる。
	this->IsNeedUpdateRule = true;

	//一番最初だけ正規表現の構文変換をかける.
    // .+ --> (:?.*)
    // (まる|さんかく)? --> (まる|さんかく|)   正しい正規表現としてはエラーだが、このエンジンの場合容認する.
    // なのは? --> なの(は|)
	std::wstring optstr = L"";

	for( const wchar_t * p = _A2W( str.c_str() ) ; *p ; ++p )
	{
        if ( *p == L'.' && *(p+1) == L'+')
        { // .+ --> (:?.*)
            optstr += L"(?:.+)";
			++p;
        }
        else if (*p == L'(' && *(p+1) == L'?' && *(p+2) == L':' )
		{
            optstr += L"(?:";
			p+=2;
		}
        else if (*(p+1) == L'?')
        {
            if (*p == L')')
            {// (まる|さんかく)? --> (まる|さんかく|)
                    optstr += L"|)";
            }
            else 
            {// なのは? --> なの(は|)
                    optstr += std::wstring(L"") + L"(?:" + *p + L"|)";
            }
			++p;
        }
        else if ( *p == L'[' && *(p+1) == L':' )
        {
			const wchar_t * end = wcsstr(p+2 , L":]");
			if (end == NULL)
			{
				return xreturn::error("[: があるのに :] がありませんでした");
			}
			optstr += std::wstring(p , (end - p) + 2);
			p = end + 1;
        }
        else if (*p == L'*' || *p == L'+' || *p == L'.' || *p == L'[' || *p == L']')
        {
			return xreturn::error(std::string("") + "現在は、メタ文字 " + _W2A(p) + " は利用できません。利用可能なメタ文字 () | .+ ?)");
        }
        else
        {
            optstr += *p;
        }
    }

	this->LocalCaptureRuleNodeCount = 1;
	if (id == UINT_MAX)
	{
		return AddRegexpImpl(NULL,optstr, stateHandle);
	}
	else
	{
		SPPROPERTYINFO prop;
		prop.pszName = L"Id";
		prop.pszValue = L"Property";
		prop.vValue.vt = VT_UINT;
		prop.vValue.ulVal = id;
		return AddRegexpImpl(&prop,optstr, stateHandle);
	}
}

//音声認識ルールを登録する部分の詳細な実行です。正規表現のネストがあるので再起してます。
xreturn::r<bool> Recognition_SAPI::AddRegexpImpl(const SPPROPERTYINFO* prop,const std::wstring & str, SPSTATEHANDLE stateHandle)
{
	_USE_WINDOWS_ENCODING;
	HRESULT hr;
    std::wstring matchString;

	//正規表現をパースしながら回す.
	const wchar_t * p = str.c_str();
    const wchar_t * splitPos = p;
    SPSTATEHANDLE currentRule = stateHandle;
	for(  ; *p ; ++p )
	{
		if (*p == L'(')
        {
            //閉じ括弧まで飛ばす. )
            int nest = 1;
            const wchar_t* n = p + 1;
            for( ; *n  ; ++n )
            {
                if (*n == L'(')
                {
                    ++nest ;
                }
                else if (*n == L')')
                {
                    --nest ;
                    if (nest <= 0)
                    {
                        break;
                    }
                }
            }

            //ネストする前の部分
            matchString = std::wstring(splitPos, 0 ,p - splitPos);


			//ネストしている部分を格納するルールを作る.
			SPSTATEHANDLE nestRule;

			//キャプチャー？
			std::wstring captureNodeName;
            if (*(p+1) == L'?' && *(p+2) == L':')
            {
                p += 2;
            }
			else
			{
				captureNodeName = num2wstr(this->LocalCaptureRuleNodeCount++) + L":" + num2wstr(this->GlobalRuleNodeCount) ;
			}

			hr = this->RuleGrammar->GetRule(  (captureNodeName.length() <= 0 ? NULL : captureNodeName.c_str())
														, this->GlobalRuleNodeCount ++ ,SRADynamic , TRUE ,  &nestRule); 
			if(FAILED(hr))	 return xreturn::windowsError(hr);

			//次の遷移へのステートが必要(ステートはルールをくっつけるためののりしろ)
			SPSTATEHANDLE nestRuleState;
			hr = this->RuleGrammar->CreateNewState(currentRule, &nestRuleState);
			if(FAILED(hr))	 return xreturn::windowsError(hr);


			//ネストする前の部分を挿入.
			hr = this->RuleGrammar->AddWordTransition(currentRule , nestRuleState , matchString.length() <= 0 ? NULL : matchString.c_str()  , L" " , SPWT_LEXICAL , 1.0f , prop );
			if(FAILED(hr))	 return xreturn::windowsError(hr);


			//かっこの後にも構文が連続する場合、そのツリーを作成する.
            if (*n == L'\0'  || *(n+1) == L'\0' || *(n+1) == L'|')
            {
                //閉じかっこで構文がとまる場合はそこで終端
				hr = this->RuleGrammar->AddRuleTransition(nestRuleState, NULL, nestRule, 1.0f , NULL);
				if(FAILED(hr))	 return xreturn::windowsError(hr);
            }
            else
            {
				SPSTATEHANDLE afterRule;
				hr = this->RuleGrammar->GetRule(NULL , this->GlobalRuleNodeCount ++ ,SRADynamic , TRUE ,  &afterRule); 
				if(FAILED(hr))	 return xreturn::windowsError(hr);

				SPSTATEHANDLE afterRuleState;
				hr = this->RuleGrammar->CreateNewState(currentRule, &afterRuleState);
				if(FAILED(hr))	 return xreturn::windowsError(hr);

				hr = this->RuleGrammar->AddRuleTransition(nestRuleState, afterRuleState, nestRule, 1.0f , NULL);
				if(FAILED(hr))	 return xreturn::windowsError(hr);

				hr = this->RuleGrammar->AddRuleTransition(afterRuleState, NULL, afterRule, 1.0f , NULL);
				if(FAILED(hr))	 return xreturn::windowsError(hr);

				currentRule = afterRule;
            }

            //ネストしているルールを再帰して実行.
			matchString = std::wstring(p+1 , 0 , (int) (n - p - 1) );
			auto r = this->AddRegexpImpl(prop,matchString, nestRule);
			if(!r)
			{
				return xreturn::errnoError(r);
			}

            p = n ;
            splitPos = n + 1;  //+1は最後の ) を飛ばす. iは forの ++i で i == splitPos となる。(わかりにくい)
        }
        else if (*p == L'|') 
        {
        	matchString = std::wstring(splitPos,0 , (int) (p - splitPos));
			if (matchString.length() >= 1)
            {
				hr = this->RuleGrammar->AddWordTransition(currentRule , NULL , matchString.c_str() , L" " , SPWT_LEXICAL , 1.0f , prop );
				if(FAILED(hr))	 return xreturn::windowsError(hr);
			}
			//空分岐 (A|) のような場合、空ノードを入れる.
			if (matchString.length() <= 0 || *(p+1) == L'\0' )
			{
				hr = this->RuleGrammar->AddWordTransition(currentRule , NULL , NULL , L" " , SPWT_LEXICAL , 1.0f , prop );
				if(FAILED(hr))	 return xreturn::windowsError(hr);
			}

			splitPos = p + 1;
            currentRule = stateHandle;
        }
        else if (*p == L'[' && *(p+1) == L':') 
        {
			//([:music:])かけて
			const wchar_t * end = wcsstr(p+2 , L":]");
			if (end == NULL)
			{
				return xreturn::error("[: があるのに :] がありませんでした");
			}
/*
			std::wstring spname = std::wstring(p+2,0,end - (p + 2));
			SPSTATEHANDLE targetHandle = NULL;
			if (spname == L"music")
			{
				targetHandle = this->MusicRuleHandle;
			}
			else if (spname == L"vedeo")
			{
				targetHandle = this->VideoRuleHandle;
			}
			else if (spname == L"book")
			{
				targetHandle = this->BookRuleHandle;
			}
			else
			{
				return xreturn::error(std::string() + "[: " + _W2A(spname.c_str()) + " :] にマッチするルールはありません");
			}

			//次の遷移へのステートが必要(ステートはルールをくっつけるためののりしろ)
//			SPSTATEHANDLE nestRuleState;
//			hr = this->RuleGrammar->CreateNewState(currentRule, &nestRuleState);
//			if(FAILED(hr))	 return xreturn::windowsError(hr);
//
//			hr = this->RuleGrammar->AddRuleTransition(currentRule, nestRuleState, targetHandle , 1.0f , NULL);
//			if(FAILED(hr))	 return xreturn::windowsError(hr);
//
//			hr = this->RuleGrammar->AddRuleTransition(nestRuleState , NULL , targetHandle , 1.0f , NULL );
//			if(FAILED(hr))	 return xreturn::windowsError(hr);

//			hr = this->RuleGrammar->AddWordTransition(currentRule , NULL , L"テスト" , L" " , SPWT_LEXICAL , 1.0f , prop );
//			if(FAILED(hr))	 return xreturn::windowsError(hr);
			SPSTATEHANDLE nestRule;
			hr = this->RuleGrammar->GetRule(  NULL, this->GlobalRuleNodeCount ++ ,SRADynamic , TRUE ,  &nestRule); 
			if(FAILED(hr))	 return xreturn::windowsError(hr);

			//次の遷移へのステートが必要(ステートはルールをくっつけるためののりしろ)
			SPSTATEHANDLE nestRuleState;
			hr = this->RuleGrammar->CreateNewState(currentRule, &nestRuleState);
			if(FAILED(hr))	 return xreturn::windowsError(hr);

			//ネストする前の部分を挿入.
			hr = this->RuleGrammar->AddWordTransition(currentRule , nestRuleState , NULL  , L" " , SPWT_LEXICAL , 1.0f , prop );
			if(FAILED(hr))	 return xreturn::windowsError(hr);

            //閉じかっこで構文がとまる場合はそこで終端
			hr = this->RuleGrammar->AddRuleTransition(nestRuleState, NULL, targetHandle, 1.0f , NULL);
			if(FAILED(hr))	 return xreturn::windowsError(hr);

//			hr = this->RuleGrammar->AddWordTransition(nestRule , NULL , L"テスト" , L" " , SPWT_LEXICAL , 1.0f , prop );
//			if(FAILED(hr))	 return xreturn::windowsError(hr);
*/
			p = end + 1;
			splitPos = p + 1;
        }
        else if (*p == L'.' && *(p+1) == L'+') 
        {
			hr = this->RuleGrammar->AddRuleTransition(currentRule, NULL, SPRULETRANS_DICTATION, 1.0f, prop);
			if(FAILED(hr))	 return xreturn::windowsError(hr);

			p += 1;
			splitPos = p + 1;
        }
    }

    //最後の残り
	matchString = std::wstring(splitPos , 0 , (int) (p - splitPos) );
    if ( matchString.length() >= 1 &&str.length() >= 1 && *(p-1) != L')')
    {
        hr = this->RuleGrammar->AddWordTransition(currentRule , NULL , matchString.c_str() , L" " , SPWT_LEXICAL , 1.0f , prop );
		if(FAILED(hr))	 return xreturn::windowsError(hr);
    }
	return true;
}

