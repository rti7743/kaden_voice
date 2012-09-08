#include "common.h"
#include "ScriptRunner.h"
#include "Recognition_Factory.h"
//#include "Recognition_SAPI.h"
#include "Recognition_JuliusPlus.h"
#include "Recognition_Cloud.h"
#include "MainWindow.h"

Recognition_Factory::Recognition_Factory()
{
	this->Engine = NULL;
}

Recognition_Factory::~Recognition_Factory()
{
	delete this->Engine;
}

bool Recognition_Factory::Create(const std::string & name , MainWindow* poolMainWindow,std::list<std::string> yobikakeListArray,std::list<std::string> cancelListArray,double temporaryRuleConfidenceFilter,const std::string& logdir)
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません
	assert(this->Engine == NULL);

	if (name == "julius")
	{
		this->Engine = new Recognition_JuliusPlus();
	}
	else if (name == "cloud")
	{
		this->Engine = new Recognition_Cloud();
	}
	else
	{
		throw XLException("音声認識エンジン" + name + "がありません");
	}
	this->Engine->Create(poolMainWindow);
	this->Engine->SetYobikake(yobikakeListArray);
	this->Engine->SetCancel(cancelListArray);
	this->Engine->SetRecognitionFilter(temporaryRuleConfidenceFilter);
	this->Engine->SetLogDirectory(logdir);

	return true;
}

bool Recognition_Factory::AddCommandRegexp(const CallbackDataStruct * callback ,const std::string p1)
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません
	this->Engine->AddCommandRegexp(callback,p1);
	return true;
}

bool Recognition_Factory::AddTemporaryRegexp(const CallbackDataStruct * callback ,const std::string p1)
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません
	this->Engine->AddTemporaryRegexp(callback,p1);
	return true;
}
bool Recognition_Factory::CommitRule()
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません

	//アップデート処理を依頼する.
	this->Engine->CommitRule();
	return true;
}
bool Recognition_Factory::ClearTemporary()
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません

	//テンポラリルールをすべて消す。
	this->Engine->ClearTemporary();
	return true;
}


bool Recognition_Factory::RemoveCallback(const CallbackDataStruct* callback , bool is_unrefCallback)
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません

	//このコールバックに関連付けられているものをすべて消す
	//一連の削除が終わったら、Commit() しないといけないよ。
	this->Engine->RemoveCallback(callback,is_unrefCallback);
	return true;
}

