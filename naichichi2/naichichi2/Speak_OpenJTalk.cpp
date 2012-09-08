// Speak_OpenJTalk.cpp: Speak_OpenJTalk クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "common.h"
#include "ScriptRunner.h"
#include "Speak_Factory.h"
#include "Speak_OpenJTalk.h"
#include "MainWindow.h"
#include "XLHttpSocket.h"
#include "XLFileUtil.h"
#include "XLStringUtil.h"
#include "XLSoundPlay.h"
#include "MecabControl.h"
#include "../openjtalk/open_jtalk/bin/open_jtalkdll.h"

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

Speak_OpenJTalk::Speak_OpenJTalk()
{
	this->Thread = NULL;
	this->StopFlag = false;
	this->CancelFlag = false;
	this->OpenJTalkHandle = NULL;
}

Speak_OpenJTalk::~Speak_OpenJTalk()
{
	this->StopFlag = true;
	this->queue_wait.notify_all();
	this->Thread->join();
	delete this->Thread;
	
	if (this->OpenJTalkHandle)
	{
		OpenJTalk_Delete(&this->OpenJTalkHandle);
		this->OpenJTalkHandle = NULL;
	}
}

//音声認識のためのオブジェクトの構築.
bool Speak_OpenJTalk::Create(MainWindow* poolMainWindow)
{
	assert(this->Thread == NULL);
	
	this->PoolMainWindow = poolMainWindow;
	this->StopFlag = false;
	
	const char* argv[]={
		"juliusplus"
		,"-C"
		,"testmic.jconf"
	};
	int argc = sizeof(argv)/sizeof(argv[0]);
	if (! OpenJTalk_Create(&this->OpenJTalkHandle
		,this->PoolMainWindow->MecabControl.getMecabHandle()
		,argc
		,argv)
	) {
//		OpenJTalk_GetLastError(&this->OpenJTalkHandle);
	}

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

void Speak_OpenJTalk::Run()
{
	_USE_WINDOWS_ENCODING;

	while(!this->StopFlag)
	{
		{
			boost::unique_lock<boost::mutex> al(this->Lock);
			if (this->SpeakQueue.size() <= 0)
			{
				this->queue_wait.wait(al);
			}
		}
		//寝起きかもしれないので終了条件の確認.
		if (this->StopFlag)
		{
			return;
		}

		//読み上げる文字列をキューから取得.
		SpeakTask task;
		{
			boost::unique_lock<boost::mutex> al(this->Lock);
			this->CancelFlag = false;

			if (this->SpeakQueue.size() <= 0)
			{
				continue;
			}
			task = *(this->SpeakQueue.begin());
			this->SpeakQueue.pop_front();
		}

//		OpenJTalk_synthesis_towav(&this->OpenJTalkHandle);

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
}

bool Speak_OpenJTalk::Setting(int rate,int pitch,unsigned int volume,const std::string& botname)
{
	//設定できません!!
	return true;
}

bool Speak_OpenJTalk::Speak(const CallbackDataStruct * callback,const std::string & str)
{
	boost::unique_lock<boost::mutex> al(this->Lock);

	//キューに積んで、読み上げスレッドに通知する.
	this->SpeakQueue.push_back(SpeakTask(callback,str));
	this->queue_wait.notify_all();

	return true;
}


bool Speak_OpenJTalk::Cancel()
{
	boost::unique_lock<boost::mutex> al(this->Lock);

	this->SpeakQueue.clear();
	this->CancelFlag = true;
	return true;
}

bool Speak_OpenJTalk::RemoveCallback(const CallbackDataStruct* callback , bool is_unrefCallback) 
{
	boost::unique_lock<boost::mutex> al(this->Lock);

	return true;
}


