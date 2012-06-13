#include "common.h"
#include "ScriptRunner.h"
#include "Speak_Factory.h"
//#include "Speak_SpeechPlatform.h"
#include "Speak_GoogleTranslate.h"
//#include "Speak_AquesTalk.h"
#include "MainWindow.h"


Speak_Factory::Speak_Factory()
{
	this->Engine = NULL;
}

Speak_Factory::~Speak_Factory()
{
	delete this->Engine;
}

xreturn::r<bool> Speak_Factory::Create(const std::string & name , MainWindow* poolMainWindow,int rate,int pitch,unsigned int volume,const std::string& botname)
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません
	assert(this->Engine == NULL);

	puts("speack");
	puts(name.c_str());
	if (name == "mssp")
	{
//		this->Engine = new Speak_SpeechPlatform();
	}
	else if (name == "google")
	{
		this->Engine = new Speak_GoogleTranslate();
	}
	else if (name == "aquestalk")
	{
//		this->Engine = new Speak_AquesTalk();
	}
	else
	{
		return xreturn::error("合成音声エンジン" + name + "がありません");
	}

	this->Engine->Create(poolMainWindow);
	this->Engine->Setting(rate,pitch,volume,botname);

	return true;
}

xreturn::r<bool> Speak_Factory::Speak(const CallbackDataStruct * callback,const std::string & str)
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません
	auto r = this->Engine->Speak(callback,str);
	if (!r) return xreturn::error(r.getError());
	return true;
}

void Speak_Factory::Cancel()
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません
	this->Engine->Cancel();
}

//このコールバックに関連付けられているものをすべて消す
xreturn::r<bool> Speak_Factory::RemoveCallback(const CallbackDataStruct* callback , bool is_unrefCallback)
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません
	auto r = this->Engine->RemoveCallback(callback,is_unrefCallback);
	if (!r) return xreturn::error(r.getError());
	return true;
}


