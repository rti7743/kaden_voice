// ActionScriptManager.cpp: ActionScriptManager クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "common.h"
#include "ScriptRunner.h"
#include "ActionScriptManager.h"
#include "MainWindow.h"
#include "XLStringUtil.h"


//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////


ActionScriptManager::ActionScriptManager()
{
}
ActionScriptManager::~ActionScriptManager()
{
}

xreturn::r<bool> ActionScriptManager::Create(MainWindow* poolMainWindow)
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません

	this->PoolMainWindow = poolMainWindow;
	return true;
}

xreturn::r<bool> ActionScriptManager::Regist(const CallbackDataStruct* callback , const std::string & actionName )
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません

	if ( actionName.find("action__") != 0)
	{
		return xreturn::error("アクション " + actionName + " の名前が正しくありません。 アクションは、 action__hogehoge のように、 action__ というprefixが必要です。 ");
	}

	std::string command = this->PoolMainWindow->Config.Get( actionName , "");
	if ( command.empty() )
	{
		return xreturn::error("アクション " + actionName + " に設定にありません。config.conf を確認してください。 ");
	}

	std::list<std::string> args = XLStringUtil::parse_command(command);
	if (args.size() <= 0)
	{
		return xreturn::error("アクション " + actionName + " の設定引数をパースしたら、引数が0になりました。内容を確認してください。 " + actionName + ": " + command);
	}

	assert(args.size() >= 1);

	std::string luafilename = XLStringUtil::pathcombine( this->PoolMainWindow->Config.GetBaseDirectory() , *(args.begin()) );

	ScriptRunner runner(this->PoolMainWindow , false );
	runner.LoadScript(luafilename);

	std::map<std::string,std::string>  data;
	data["action_result"] = runner.callFunction("call",args,true);

	std::string respons;
	this->PoolMainWindow->ScriptManager.ActionEnd( callback , data );

//	::boost::this_thread::sleep(1);

	return true;
}

