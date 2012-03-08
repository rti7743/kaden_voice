// SAPISpeechRecognition.h: SAPISpeechRecognition クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Speak_GoogleTranslate_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_)
#define AFX_Speak_GoogleTranslate_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Speak_GoogleTranslate : public ISpeechSpeakInterface
{
public:
	Speak_GoogleTranslate();
	virtual ~Speak_GoogleTranslate	();

	//音声のためのオブジェクトの構築.
	virtual xreturn::r<bool> Speak_GoogleTranslate::Create(MainWindow* poolMainWindow);
	virtual xreturn::r<bool> Speak_GoogleTranslate::Setting(int rate,int pitch,unsigned int volume,const std::string& botname);
	virtual xreturn::r<bool> Speak_GoogleTranslate::Speak(const std::string & str);
	virtual xreturn::r<bool> Speak_GoogleTranslate::RegistWaitCallback(const CallbackDataStruct * callback);
	virtual xreturn::r<bool> Speak_GoogleTranslate::Cancel();
	virtual xreturn::r<bool> Speak_GoogleTranslate::RemoveCallback(const CallbackDataStruct* callback , bool is_unrefCallback) ;

private:
	void Speak_GoogleTranslate::Run();

	std::list<std::string> SpeakQueue;
	MainWindow* PoolMainWindow;

	boost::thread* Thread;
	boost::mutex   Lock;
	boost::condition_variable queue_wait;
	bool           StopFlag;

	std::vector<const CallbackDataStruct*> CallbackDictionary;
};

#endif // !defined(AFX_Speak_GoogleTranslate_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_)
