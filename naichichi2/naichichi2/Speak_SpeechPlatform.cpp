// Speak_SpeechPlatform.cpp: Speak_SpeechPlatform クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "common.h"
#include "ScriptRunner.h"
#include "Speak_Factory.h"
#include "Speak_SpeechPlatform.h"
#include "MainWindow.h"
//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////
Speak_SpeechPlatform::Speak_SpeechPlatform()
{
	this->Thread = NULL;
	this->StopFlag = false;
	this->CancelFlag = false;
}

Speak_SpeechPlatform::~Speak_SpeechPlatform()
{
	this->StopFlag = true;
	this->queue_wait.notify_all();
	this->Thread->join();
	delete this->Thread;
}

//音声認識のためのオブジェクトの構築.
bool Speak_SpeechPlatform::Create(MainWindow* poolMainWindow)
{
	assert(this->Thread == NULL);
	
	this->PoolMainWindow = poolMainWindow;
	this->StopFlag = false;
	this->Thread = new boost::thread([=](){
		try
		{
			this->Run(); 
		}
		catch(XLException &e)
		{
			this->PoolMainWindow->SyncInvokeError( e.getErrorMessage() );
		}
	} );
	return true;
}


bool Speak_SpeechPlatform::Run()
{
	_USE_WINDOWS_ENCODING;

	//comの初期化
	COMInit cominit;

	//エンジンの構築
	HRESULT hr;

	hr = this->Engine.CoCreateInstance(CLSID_SpVoice);
	if(FAILED(hr))	 throw XLException(XLException::StringWindows(hr));

	//ボットを登録する
	this->RegistVoiceBot("");

	//http://msdn.microsoft.com/en-us/library/ms720164(v=vs.85).aspx
	hr = this->Engine->SetInterest(SPFEI(SPEI_END_INPUT_STREAM), SPFEI(SPEI_END_INPUT_STREAM));
	if(FAILED(hr))	 throw XLException(XLException::StringWindows(hr));

	//文章が来たら読み上げる
	while(!this->StopFlag)
	{
		{
			boost::unique_lock<boost::mutex> al(this->Lock);
			this->CancelFlag = false;
			if (this->SpeakQueue.size() <= 0)
			{
				this->queue_wait.wait(al);
			}
		}
		//寝起きかもしれないので終了条件の確認.
		if (this->StopFlag)
		{
			return true;
		}

		//読み上げる文字列をキューから取得.
		SpeakTask task;
		{
			boost::unique_lock<boost::mutex> al(this->Lock);

			if (this->SpeakQueue.size() <= 0)
			{
				continue;
			}
			task = *(this->SpeakQueue.begin());
			this->SpeakQueue.pop_front();
		}
		hr = this->Engine->Speak( _A2W(task.text.c_str()) ,  SVSFIsXML,NULL);
		if(FAILED(hr))	throw XLException(XLException::StringWindows(hr));
		
		if (this->CancelFlag)
		{

		}
		else
		{
			//コールバックする.
			this->PoolMainWindow->AsyncInvoke( [=](){
				this->PoolMainWindow->ScriptManager.SpeakEnd(task.callback,task.text);
			} );
		}
	}
	return true;
}

bool Speak_SpeechPlatform::Setting(int rate,int pitch,unsigned int volume,const std::string& botname)
{
//	HRESULT hr;

//	hr = this->Engine->SetRate(rate);
//	if(FAILED(hr))	 throw XLException(XLException::StringWindows(hr));

//	hr = this->Engine->SetVolume(volume);
//	if(FAILED(hr))	 throw XLException(XLException::StringWindows(hr));

//	this->Pitch = pitch;

	return true;
}

bool Speak_SpeechPlatform::RegistVoiceBot(const std::string & botname)
{
	//see http://msdn.microsoft.com/en-us/library/ms719807(v=vs.85).aspxs
	_USE_WINDOWS_ENCODING;
	HRESULT hr;

	CComPtr<IEnumSpObjectTokens>   cpEnum;
	hr = SpEnumTokens(SPCAT_VOICES, NULL, NULL, &cpEnum);
	if(FAILED(hr))	 throw XLException(XLException::StringWindows(hr));

	ULONG ulCount;
	hr = cpEnum->GetCount(&ulCount);
	if(FAILED(hr))	 throw XLException(XLException::StringWindows(hr));

	std::string foundBotName = "";
	while (SUCCEEDED(hr) && ulCount--)
	{
		CComPtr<ISpObjectToken>        cpVoiceToken;
		hr = cpEnum->Next(1, &cpVoiceToken, NULL);
		if(FAILED(hr))	 throw XLException(XLException::StringWindows(hr));

		CSpDynamicString curDesc;
		hr = SpGetDescription(cpVoiceToken, &curDesc);
		if(FAILED(hr))	 throw XLException(XLException::StringWindows(hr));

		if (botname.empty() || strstr(_W2A(curDesc),botname.c_str()) != NULL )
		{
			hr = this->Engine->SetVoice(cpVoiceToken);
			if(FAILED(hr))	 throw XLException(XLException::StringWindows(hr));

			return true;
		}
		else
		{
			foundBotName = foundBotName + _W2A(curDesc) + " ";
		}
	}
	throw XLException("読み上げるボット" + botname + "がみつかりません。見つかったボット:" + foundBotName);
}


bool Speak_SpeechPlatform::Speak(const CallbackDataStruct * callback,const std::string & str)
{
	boost::unique_lock<boost::mutex> al(this->Lock);

	//キューに積んで、読み上げスレッドに通知する.
	this->SpeakQueue.push_back(SpeakTask(callback,str));
	this->queue_wait.notify_all();

	return true;
}


bool Speak_SpeechPlatform::Cancel()
{
	boost::unique_lock<boost::mutex> al(this->Lock);

	this->SpeakQueue.clear();
	this->CancelFlag = true;
	return true;
}

bool Speak_SpeechPlatform::RemoveCallback(const CallbackDataStruct* callback , bool is_unrefCallback) 
{
	boost::unique_lock<boost::mutex> al(this->Lock);

	return true;
}



