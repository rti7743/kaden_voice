// SAPISpeechRecognition.h: SAPISpeechRecognition クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Speak_OpenJTalk_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_)
#define AFX_Speak_OpenJTalk_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
struct OpenJTalk;

class Speak_OpenJTalk : public ISpeechSpeakInterface
{
public:
	Speak_OpenJTalk();
	virtual ~Speak_OpenJTalk	();

	//音声のためのオブジェクトの構築.
	virtual bool Create(MainWindow* poolMainWindow);
	virtual bool Setting(int rate,int pitch,unsigned int volume,const std::string& botname);
	virtual bool Speak(const CallbackDataStruct * callback,const std::string & str);
	virtual bool Cancel();
	virtual bool RemoveCallback(const CallbackDataStruct* callback , bool is_unrefCallback) ;

private:
	void Run();

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

	boost::thread* Thread;
	boost::mutex   Lock;
	boost::condition_variable queue_wait;
	bool           StopFlag;
	bool		   CancelFlag;

	OpenJTalk*     OpenJTalkHandle;
};

#endif // !defined(AFX_Speak_OpenJTalk_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_)
