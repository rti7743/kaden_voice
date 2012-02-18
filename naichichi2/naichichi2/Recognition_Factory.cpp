#include "common.h"
#include "ScriptRunner.h"
#include "Recognition_Factory.h"
#include "Recognition_SAPI.h"
#include "MainWindow.h"

Recognition_Factory::Recognition_Factory()
{
	this->Engine = NULL;
}

Recognition_Factory::~Recognition_Factory()
{
	delete this->Engine;
}

xreturn::r<bool> Recognition_Factory::Create(const std::string & name , MainWindow* poolMainWindow,std::list<std::string> yobikakeListArray,double temporaryRuleConfidenceFilter,double yobikakeRuleConfidenceFilter,double basicRuleConfidenceFilter,bool dictation_Filter )
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません
	assert(this->Engine == NULL);

	if (name == "sapi")
	{
		this->Engine = new Recognition_SAPI();
	}
	else
	{
		return xreturn::error("音声認識エンジン" + name + "がありません");
	}
	this->Engine->Create(poolMainWindow);
	this->Engine->SetYobikake(yobikakeListArray);
	this->Engine->SetRecognitionFilter(temporaryRuleConfidenceFilter,yobikakeRuleConfidenceFilter,basicRuleConfidenceFilter,dictation_Filter);

	return true;
}

xreturn::r<bool> Recognition_Factory::AddCommandRegexp(CallbackDataStruct & callback ,const std::string p1)
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません
	auto r = this->Engine->AddCommandRegexp(callback,p1);
	if (!r) return xreturn::error(r.getError());
	return true;
}

xreturn::r<bool> Recognition_Factory::AddTemporaryRegexp(CallbackDataStruct & callback ,const std::string p1)
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません
	auto r = this->Engine->AddTemporaryRegexp(callback,p1);
	if (!r) return xreturn::error(r.getError());
	return true;
}
xreturn::r<bool> Recognition_Factory::CommitRule()
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません

	//アップデート処理を依頼する.
	auto r = this->Engine->CommitRule();
	if (!r) return xreturn::error(r.getError());
	return true;
}
xreturn::r<bool> Recognition_Factory::ClearTemporary()
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません

	//テンポラリルールをすべて消す。
	auto r = this->Engine->ClearTemporary();
	if (!r) return xreturn::error(r.getError());
	return true;
}
