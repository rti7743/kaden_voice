// Speak_GoogleTranslate.cpp: Speak_GoogleTranslate クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "common.h"
#include "ScriptRunner.h"
#include "Speak_Factory.h"
#include "Speak_GoogleTranslate.h"
#include "MainWindow.h"
#include "XLHttpRequerst.h"
#include "XLStringUtil.h"
#include "XLSoundPlay.h"

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

Speak_GoogleTranslate::Speak_GoogleTranslate()
{
	this->Thread = NULL;
	this->StopFlag = false;
}

Speak_GoogleTranslate::~Speak_GoogleTranslate()
{
	this->StopFlag = true;
	this->queue_wait.notify_all();
	this->Thread->join();
	delete this->Thread;
}

//音声認識のためのオブジェクトの構築.
xreturn::r<bool> Speak_GoogleTranslate::Create(MainWindow* poolMainWindow)
{
	assert(this->Thread == NULL);
	
	this->PoolMainWindow = poolMainWindow;
	this->StopFlag = false;
	this->Thread = new boost::thread([=](){
		try
		{
			this->Run(); 
		}
		catch(xreturn::error &e)
		{
			this->PoolMainWindow->SyncInvokeError( e.getErrorMessage() );
		}
	} );
	return true;
}

void Speak_GoogleTranslate::Run()
{
	_USE_WINDOWS_ENCODING;

	while(!this->StopFlag)
	{
/*
		bool istask = false;
		//キューがあるかどうか確認.
		{
			boost::unique_lock<boost::mutex> al(this->Lock);
			istask = (this->SpeakQueue.size() >= 1);
		}
		//タスクがないならば、タスクが入るまで寝る.
		if (!istask)
		{
			
		}
*/
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
		std::string url;
		{
			boost::unique_lock<boost::mutex> al(this->Lock);

			if (this->SpeakQueue.size() <= 0)
			{
				continue;
			}
			url = *(this->SpeakQueue.begin());
			this->SpeakQueue.pop_front();
		}
		//ダウンロードの準備
		std::string lang = "ja";
		std::string encoding = "utf_8";	

		XLHttpRequerst downloader;
		std::map<std::string , std::string> param;
		param["q"]		= _A2U(url.c_str());
		param["hl"]		= lang;
		param["lr"]		= std::string("") + "lang_" + lang;
		param["ie"]		= encoding;
		param["oe"]		= encoding;

		std::map<std::string,std::string> header;
		std::map<std::string,std::string> option;

		header["user-agent"] = "Mozilla/5.0 (Windows; U; Windows NT 5.1; ja-ｊｐ) AppleWebKit/533.4 (KHTML, like Gecko) Chrome/5.0.375.70 Safari/533.4";
		downloader.Download("http://translate.google.com/translate_tts?" + XLStringUtil::crossjoin("=","&",param) ,header,option);

		if (this->StopFlag)
		{
			return;
		}

		//テンポラリに一度出力する.
		const char * tempfilename = tempnam(NULL,"naichichi_");
		if (tempfilename == NULL)
		{
			continue;
		}
		std::string filename = std::string(tempfilename) + ".mp3";
		downloader.saveFile(filename);

		free((void*)tempfilename);
		tempfilename = NULL;

		XLSoundPlay soundplay;
		soundplay.play(filename);

		unlink( filename.c_str() );
		if (this->StopFlag)
		{
			return;
		}

		//キューを消化していたらコールバックする.
		{
			boost::unique_lock<boost::mutex> al(this->Lock);

			if (this->SpeakQueue.size() <= 0)
			{
				this->PoolMainWindow->SyncInvoke( [&](){
					for(auto it = this->CallbackDictionary.begin(); this->CallbackDictionary.end() != it ; ++it )
					{
						this->PoolMainWindow->ScriptManager.SpeakEnd(*it);
					}
					this->CallbackDictionary.clear();
				} );

			}
		}
	}
}

xreturn::r<bool> Speak_GoogleTranslate::Setting(int rate,int pitch,unsigned int volume,const std::string& botname)
{
	//設定できません!!
	return true;
}

xreturn::r<bool> Speak_GoogleTranslate::Speak(const std::string & str)
{
	boost::unique_lock<boost::mutex> al(this->Lock);

	//キューに積んで、読み上げスレッドに通知する.
	this->SpeakQueue.push_back(str);
	this->queue_wait.notify_all();

	return true;
}

xreturn::r<bool> Speak_GoogleTranslate::RegistWaitCallback(CallbackDataStruct & callback)
{
	boost::unique_lock<boost::mutex> al(this->Lock);

	this->CallbackDictionary.push_back(callback);

	//一応、キュー街がないかどうか確認してもらおう.
	this->queue_wait.notify_all();

	return true;
}

xreturn::r<bool> Speak_GoogleTranslate::Cancel()
{
	boost::unique_lock<boost::mutex> al(this->Lock);

	this->SpeakQueue.clear();
	return true;
}

