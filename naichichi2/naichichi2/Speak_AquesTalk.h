// SAPISpeechRecognition.h: SAPISpeechRecognition クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Speak_AquesTalk_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_)
#define AFX_Speak_AquesTalk_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "LoadLibraryHelper.h"

class Speak_AquesTalk : public ISpeechSpeakInterface
{
public:
	Speak_AquesTalk();
	virtual ~Speak_AquesTalk();

	//音声のためのオブジェクトの構築.
	virtual xreturn::r<bool> Speak_AquesTalk::Create(MainWindow* poolMainWindow);
	virtual xreturn::r<bool> Speak_AquesTalk::Setting(int rate,int pitch,unsigned int volume,const std::string& botname);
	virtual xreturn::r<bool> Speak_AquesTalk::Speak(const CallbackDataStruct * callback,const std::string & str);
	virtual xreturn::r<bool> Speak_AquesTalk::Cancel();
	virtual xreturn::r<bool> Speak_AquesTalk::RemoveCallback(const CallbackDataStruct* callback , bool is_unrefCallback) ;

private:
	void Speak_AquesTalk::Run();

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
	std::vector<char> PhontDat; //aques talk2 の場合音声データを選べる。　emptyならディフォルト音声

	boost::thread* Thread;
	boost::mutex   Lock;
	boost::condition_variable queue_wait;
	bool           StopFlag;
	bool			CancelFlag;

	//AquesTalkを何とかして読み込む
	xreturn::r<bool> Speak_AquesTalk::LoadAquesTalk();
	//バージョンを取得.
	int Speak_AquesTalk::getVersion() const;

	LoadLibraryHelper Lib;

	//for aques talk2
	typedef unsigned char *  ( __stdcall *AquesTalk2_SyntheDef)(const char *koe, int iSpeed, int *pSize, void *phontDat);
	typedef void  ( __stdcall *AquesTalk2_FreeDef)(unsigned char *wav);
	AquesTalk2_SyntheDef	AquesTalk2_Synthe;
	AquesTalk2_FreeDef		AquesTalk2_Free;

	//for aques talk1
	typedef unsigned char *  ( __stdcall *AquesTalk_SyntheDef)(const char *koe, int iSpeed, int *pSize);
	typedef void  ( __stdcall *AquesTalk_FreeWaveDef)(unsigned char *wav);
	AquesTalk_SyntheDef		AquesTalk_Synthe;
	AquesTalk_FreeWaveDef	AquesTalk_FreeWave;
};

#endif // !defined(AFX_Speak_AquesTalk_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_)
