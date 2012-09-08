// SAPISpeechRecognition.h: SAPISpeechRecognition クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SAPISpeechRecognition_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_)
#define AFX_SAPISpeechRecognition_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <atlbase.h>

//Speech Platform SDK 入れてね http://www.microsoft.com/download/en/details.aspx?id=27226
#include <../../../Speech/v11.0/Include/sapi.h>
#include <../../../Speech/v11.0/Include/sphelper.h>



class Speak_SpeechPlatform : public ISpeechSpeakInterface
{
public:
	Speak_SpeechPlatform();
	virtual ~Speak_SpeechPlatform	();

	//音声のためのオブジェクトの構築.
	virtual bool Create(MainWindow* poolMainWindow);
	virtual bool Setting(int rate,int pitch,unsigned int volume,const std::string& botname);
	virtual bool Speak(const CallbackDataStruct * callback,const std::string & str);
	virtual bool Cancel();
	virtual bool RemoveCallback(const CallbackDataStruct* callback , bool is_unrefCallback) ;

private:
	bool Run();
	bool RegistVoiceBot(const std::string & botname);

	struct SpeakTask
	{
		SpeakTask()
		{
		}
		SpeakTask(const CallbackDataStruct * callback,const std::string & str) : text(str) , callback(callback)
		{
		}

		std::string text;
		const CallbackDataStruct*	callback;
	};
	std::list<SpeakTask> SpeakQueue;
	MainWindow* PoolMainWindow;

	CComPtr<ISpVoice>				Engine;

	boost::thread* Thread;
	boost::mutex   Lock;
	boost::condition_variable queue_wait;
	bool           StopFlag;
	bool		   CancelFlag;
};

#endif // !defined(AFX_SAPISpeechRecognition_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_)
