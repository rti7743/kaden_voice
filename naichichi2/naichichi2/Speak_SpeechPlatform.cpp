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
	this->isSpeakingFlg = false;
}

Speak_SpeechPlatform::~Speak_SpeechPlatform()
{

}

//音声認識のためのオブジェクトの構築.
xreturn::r<bool> Speak_SpeechPlatform::Create(MainWindow* poolMainWindow)
{
	HRESULT hr;

	hr = this->Engine.CoCreateInstance(CLSID_SpVoice);
	if(FAILED(hr))	 return xreturn::windowsError(hr);

	//http://msdn.microsoft.com/en-us/library/ms720164(v=vs.85).aspx
	hr = this->Engine->SetInterest(SPFEI(SPEI_END_INPUT_STREAM), SPFEI(SPEI_END_INPUT_STREAM));
	if(FAILED(hr))	 return xreturn::windowsError(hr);

	this->isSpeakingFlg = false;
	this->Pitch = 0;

	return true;
}

xreturn::r<bool> Speak_SpeechPlatform::Setting(int rate,int pitch,unsigned int volume,const std::string& botname)
{
	HRESULT hr;

//	hr = this->Engine->SetRate(rate);
//	if(FAILED(hr))	 return xreturn::windowsError(hr);

//	hr = this->Engine->SetVolume(volume);
//	if(FAILED(hr))	 return xreturn::windowsError(hr);

	this->Pitch = pitch;

	//ボットを登録する
	return this->RegistVoiceBot(botname);
}
xreturn::r<bool> Speak_SpeechPlatform::RegistVoiceBot(const std::string & botname)
{
	//see http://msdn.microsoft.com/en-us/library/ms719807(v=vs.85).aspxs
	_USE_WINDOWS_ENCODING;
	HRESULT hr;

	RSimpleComPtr<IEnumSpObjectTokens>   cpEnum;
	hr = SpEnumTokens(SPCAT_VOICES, NULL, NULL, &cpEnum);
	if(FAILED(hr))	 return xreturn::windowsError(hr);

	ULONG ulCount;
	hr = cpEnum->GetCount(&ulCount);
	if(FAILED(hr))	 return xreturn::windowsError(hr);

	std::string foundBotName = "";
	while (SUCCEEDED(hr) && ulCount--)
	{
		RSimpleComPtr<ISpObjectToken>        cpVoiceToken;
		hr = cpEnum->Next(1, &cpVoiceToken, NULL);
		if(FAILED(hr))	 return xreturn::windowsError(hr);

		CSpDynamicString curDesc;
		hr = SpGetDescription(cpVoiceToken, &curDesc);
		if(FAILED(hr))	 return xreturn::windowsError(hr);

		if (botname.empty() || strstr(_W2A(curDesc),botname.c_str()) != NULL )
		{
			hr = this->Engine->SetVoice(cpVoiceToken);
			if(FAILED(hr))	 return xreturn::windowsError(hr);

			return true;
		}
		else
		{
			foundBotName = foundBotName + _W2A(curDesc) + " ";
		}
	}
	return xreturn::error("読み上げるボット" + botname + "がみつかりません。見つかったボット:" + foundBotName);
}

xreturn::r<bool> Speak_SpeechPlatform::Speak(const std::string & str)
{
	//話し中ならばキューに積む.
	if ( this->isSpeakingFlg )
	{
		this->SpeakQueue.push_back(str);
		return true;
	}

	HRESULT hr;
	_USE_WINDOWS_ENCODING;
	hr = this->Engine->Speak(_A2W(str.c_str()) , SVSFlagsAsync | SVSFIsXML,NULL);
	if(FAILED(hr))	 return xreturn::windowsError(hr);

	this->isSpeakingFlg = true;
	return true;
}

xreturn::r<bool> Speak_SpeechPlatform::RegistWaitCallback(CallbackDataStruct & callback)
{
	this->CallbackDictionary.push_back(callback);
	if (!this->isSpeakingFlg)
	{
		//今喋っていないなら、一気にコールバックする.
		this->FireWaitCallback();
	}
	return true;
}

xreturn::r<bool> Speak_SpeechPlatform::Cancel()
{
	HRESULT hr;

	this->SpeakQueue.clear();

	hr = this->Engine->Pause();
	if(FAILED(hr))	 return xreturn::windowsError(hr);

	return true;
}

void Speak_SpeechPlatform::Callback(WPARAM wParam, LPARAM lParam)
{
	HRESULT hr;
	_USE_WINDOWS_ENCODING;

	if ( this->SpeakQueue.size() > 0 )
	{
		std::string str = *(this->SpeakQueue.begin());
		this->SpeakQueue.pop_front();

		hr = this->Engine->Speak( _A2W(str.c_str()) , SVSFlagsAsync | SVSFIsXML,NULL);
		if(FAILED(hr))	xreturn::windowsError(hr);

		return ;
	}

	if ( this->SpeakQueue.size() <= 0 )
	{
		this->isSpeakingFlg = false;
		this->FireWaitCallback();
		return ;
	}
}

void Speak_SpeechPlatform::FireWaitCallback()
{
	for(auto it = this->CallbackDictionary.begin(); this->CallbackDictionary.end() != it ; ++it )
	{
		this->PoolMainWindow->ScriptManager.SpeakEnd( *it );
	}
	this->CallbackDictionary.clear();
}

