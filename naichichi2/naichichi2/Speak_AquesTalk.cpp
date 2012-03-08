// Speak_AquesTalk.cpp: Speak_AquesTalk クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "common.h"
#include "ScriptRunner.h"
#include "Speak_Factory.h"
#include "Speak_AquesTalk.h"
#include "MainWindow.h"
#include "XLHttpRequerst.h"
#include "XLStringUtil.h"
#include "XLFileUtil.h"
#include "XLSoundPlay.h"

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

Speak_AquesTalk::Speak_AquesTalk()
{
	this->Thread = NULL;
	this->StopFlag = false;
}

Speak_AquesTalk::~Speak_AquesTalk()
{
	this->StopFlag = true;
	this->queue_wait.notify_all();
	this->Thread->join();
	delete this->Thread;
}

//音声認識のためのオブジェクトの構築.
xreturn::r<bool> Speak_AquesTalk::Create(MainWindow* poolMainWindow)
{
	assert(this->Thread == NULL);
	
	this->PoolMainWindow = poolMainWindow;
	auto r = this->LoadAquesTalk();
	if ( !r  )
	{
		return xreturn::error(r.getError());
	}

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

//AquesTalkを何とかして読み込む
xreturn::r<bool> Speak_AquesTalk::LoadAquesTalk()
{
	this->AquesTalk_FreeWave = NULL;
	this->AquesTalk_Synthe = NULL;
	this->AquesTalk2_Synthe = NULL;
	this->AquesTalk2_Free = NULL;
	const std::string basepath = this->PoolMainWindow->Config.GetBaseDirectory();

#ifdef _WINDOWS
	std::string aquesTalk = "AquesTalk2.dll";
	std::string aquesTalk2 = "AquesTalk.dll";
#else
	std::string aquesTalk = "AquesTalk2.so";
	std::string aquesTalk2 = "AquesTalk.so";
#endif
	if (!  this->Lib.Load(basepath + "\\" + aquesTalk2) )
	{
		this->AquesTalk2_Synthe =  (AquesTalk2_SyntheDef) this->Lib.GetProcAddress("AquesTalk2_Synthe");
		this->AquesTalk2_Free = (AquesTalk2_FreeDef) this->Lib.GetProcAddress("AquesTalk2_Free");
		return true;
	}
	if (!  this->Lib.Load(aquesTalk2) )
	{
		this->AquesTalk2_Synthe = (AquesTalk2_SyntheDef) this->Lib.GetProcAddress("AquesTalk2_Synthe");
		this->AquesTalk2_Free = (AquesTalk2_FreeDef) this->Lib.GetProcAddress("AquesTalk2_Free");
		return true;
	}

	if (!  this->Lib.Load(basepath + "\\" + aquesTalk )  )
	{
		this->AquesTalk_Synthe = (AquesTalk_SyntheDef) this->Lib.GetProcAddress("AquesTalk_Synthe");
		this->AquesTalk_FreeWave = (AquesTalk_FreeWaveDef) this->Lib.GetProcAddress("AquesTalk_FreeWave");
		return true;
	}
	if (!  this->Lib.Load(aquesTalk) )
	{
		this->AquesTalk_Synthe = (AquesTalk_SyntheDef) this->Lib.GetProcAddress("AquesTalk_Synthe");
		this->AquesTalk_FreeWave = (AquesTalk_FreeWaveDef) this->Lib.GetProcAddress("AquesTalk_FreeWave");
		return true;
	}

	return xreturn::error(aquesTalk2 + " または " + aquesTalk + " が読み込めません。" );
}


void Speak_AquesTalk::Run()
{
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
		std::string word;
		{
			boost::unique_lock<boost::mutex> al(this->Lock);

			if (this->SpeakQueue.size() <= 0)
			{
				continue;
			}
			word = *(this->SpeakQueue.begin());
			this->SpeakQueue.pop_front();
		}

		//aques talkにwaveを生成させる。
		int size;
		int speed = 1;
		unsigned char * wavData = NULL;
		if (getVersion() == 1)
		{
			wavData = this->AquesTalk_Synthe(word.c_str(), speed ,&size  );
		}
		else 
		{
			wavData = this->AquesTalk2_Synthe(word.c_str(), speed ,&size , (void*) (this->PhontDat.empty() ? NULL : &this->PhontDat[0])  );
		}
		if (wavData == NULL)
		{//エラー
			continue;
		}

		//テンポラリに一度出力する.
		const char * tempfilename = tempnam(NULL,"naichichi_");
		if (tempfilename == NULL)
		{
			continue;
		}
		std::string filename = std::string(tempfilename) + ".wav";
		XLFileUtil::write(filename,(const char*)wavData,size);
		free((void*)tempfilename);
		tempfilename = NULL;

		//AquesTalkのデータを開放する.
		if (getVersion() == 1)
		{
			this->AquesTalk2_Free(wavData);
		}
		else 
		{
			this->AquesTalk_FreeWave(wavData);
		}
		wavData = NULL;

		//再生する.
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

xreturn::r<bool> Speak_AquesTalk::Setting(int rate,int pitch,unsigned int volume,const std::string& botname)
{
	if (getVersion() == 2)
	{
		if (!botname.empty())
		{
			std::string botname_ = botname;
			if ( XLStringUtil::baseext(botname) != ".phont" )
			{
				botname_ += ".phont";
			}
			const std::string basepath = this->PoolMainWindow->Config.GetBaseDirectory();
			if (! XLFileUtil::cat_b(basepath + "\\phont\\" + botname_ , &this->PhontDat) )
			{
				if (! XLFileUtil::cat_b(basepath + "\\" + botname_ , &this->PhontDat) )
				{
					if (! XLFileUtil::cat_b(botname_ , &this->PhontDat) )
					{
						//ないので、ディフォルトを使おう!
						this->PhontDat.clear();
						this->PoolMainWindow->SyncInvokeError("Speak_AquesTalk 指定された、音声データ " + botname + "を読み込むことができませんでした" );
					}
				}
			}
		}
	}
	return true;
}

xreturn::r<bool> Speak_AquesTalk::Speak(const std::string & str)
{
	boost::unique_lock<boost::mutex> al(this->Lock);

	//キューに積んで、読み上げスレッドに通知する.
	this->SpeakQueue.push_back(str);
	this->queue_wait.notify_all();

	return true;
}

xreturn::r<bool> Speak_AquesTalk::RegistWaitCallback(const CallbackDataStruct * callback)
{
	boost::unique_lock<boost::mutex> al(this->Lock);

	this->CallbackDictionary.push_back(callback);

	//一応、キュー街がないかどうか確認してもらおう.
	this->queue_wait.notify_all();

	return true;
}

xreturn::r<bool> Speak_AquesTalk::Cancel()
{
	boost::unique_lock<boost::mutex> al(this->Lock);

	this->SpeakQueue.clear();
	return true;
}

int Speak_AquesTalk::getVersion() const
{
	return this->AquesTalk2_Synthe == NULL ? 1 : 2;
}

xreturn::r<bool> Speak_AquesTalk::RemoveCallback(const CallbackDataStruct* callback , bool is_unrefCallback) 
{
	CRemoveIF(this->CallbackDictionary , {
		if (_ == callback)
		{
			return false; //消す.
		}
	});
	return true;
}

