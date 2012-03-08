// TriggerManager.cpp: TriggerManager クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "common.h"
#include "ScriptRunner.h"
#include "TriggerManager.h"
#include "MainWindow.h"
#include "XLStringUtil.h"


//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////


TriggerManager::TriggerManager()
{
}
TriggerManager::~TriggerManager()
{
	for(auto it = this->TriggerMap.begin() ; it != this->TriggerMap.end() ; ++it )
	{
		delete it->second;
	}
}

xreturn::r<bool> TriggerManager::Create(MainWindow* poolMainWindow)
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません

	this->PoolMainWindow = poolMainWindow;
	return true;
}

xreturn::r<bool> TriggerManager::Regist(const CallbackDataStruct* callback , const std::string & menuName,const std::string & triggerName )
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません

	//すでにあるならメモリ解放しといてね
	auto alreadyIT = this->TriggerMap.find(triggerName);
	if ( alreadyIT != this->TriggerMap.end() )
	{
		delete alreadyIT->second;

		this->PoolMainWindow->SyncInvokeLog(std::string() + "Trigger " + triggerName + " は、既に存在しますが、新しい値で上書きされます。" ,LOG_LEVEL_WARNING);
	}

	//追加(または上書き)
	this->TriggerMap[triggerName] = new triggerStruct(menuName,callback);
	return true;
}

xreturn::r<std::string> TriggerManager::Call(const std::string & triggerName ,const std::map<std::string,std::string>& args)
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません

	for(auto it = this->TriggerMap.begin() ; it != this->TriggerMap.end() ; ++it )
	{
		if ( it->first == triggerName )
		{
			std::string respons;
			this->PoolMainWindow->ScriptManager.TriggerCall(it->second->callback,args,&respons);
			return respons;
		}
	}

	return xreturn::error("指定された" + triggerName + "は存在しません");
}

bool TriggerManager::IsExist(const std::string & triggerName) const
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません

	auto alreadyIT = this->TriggerMap.find(triggerName);
	return alreadyIT != this->TriggerMap.end();
}

std::map<std::string,std::string> TriggerManager::GetAllMenuname() const
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません

	std::map<std::string,std::string> d;
	for(auto it = this->TriggerMap.begin() ; it != this->TriggerMap.end() ; ++it )
	{
		if (! it->second->menuname.empty() )
		{
			d.insert( std::pair<std::string,std::string>(it->first,it->second->menuname) );
		}
	}
	return d;
}