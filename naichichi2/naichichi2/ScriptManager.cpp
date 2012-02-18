#include "common.h"
#include <io.h>
#include "ScriptManager.h"
#include "MainWindow.h"

//作成
xreturn::r<bool> ScriptManager::Create(MainWindow * poolMainWindow)
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません
	this->PoolMainWindow = poolMainWindow;
	this->PoolMainWindow->SyncInvokeLog("ScriptManager初期化開始",LOG_LEVEL_DEBUG);

	this->loadLua(this->PoolMainWindow->Config.GetBaseDirectory() );
	this->RunAllLua();
	this->PoolMainWindow->Recognition.CommitRule();

	this->PoolMainWindow->SyncInvokeLog("ScriptManager初期化完了",LOG_LEVEL_DEBUG);
	return true;
}

ScriptManager::~ScriptManager()
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません

	this->destoryLua();
}

//luaファイル郡の読み込み
xreturn::r<bool> ScriptManager::loadLua(const std::string & baseDirectory)
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません

	long handle;
	_finddata_t data;
	std::string luafind = baseDirectory + "\\scenario_*.lua";

	this->Scripts.clear();

	handle = _findfirst( luafind.c_str() , &data );
	if (handle == -1)
	{
		return xreturn::error("findfirstに失敗 ディレクトリ:" + luafind + " を検索できません。",-1);
	}
	do
	{
		const std::string luafilename = baseDirectory + "\\" + data.name;

		ScriptRunner* runner = new ScriptRunner(this->PoolMainWindow , true );
		auto r1 = runner->LoadScript(luafilename);
		if (!r1)
		{
			this->PoolMainWindow->SyncInvokeError( r1.getFullErrorMessage() );
			continue;
		}

		this->Scripts.push_back(runner);
	}
	while( ! _findnext(handle,&data) );
	
	_findclose(handle);
	return true;
}


//luaの実行
void ScriptManager::RunAllLua()
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません

	auto it = this->Scripts.begin();
	for( ; it != this->Scripts.end() ; ++it )
	{
		auto r = (*it)->callFunction("call");
		if (!r)
		{
			this->PoolMainWindow->SyncInvokeError( r.getFullErrorMessage() );
			continue;
		}
	}
}

//luaの終了
void ScriptManager::destoryLua()
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません

	auto it = this->Scripts.begin();
	for( ; it != this->Scripts.end() ; ++it )
	{
		delete *it;
	}
	this->Scripts.clear();
}


//音声認識に失敗した時の結果
void ScriptManager::BadVoiceRecogntion(int errorCode,const std::string& matString,const std::string& diction,double yobikakeRuleConfidenceFilter,double basicRuleConfidenceFilter,bool dictationCheck)
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません
	this->PoolMainWindow->SyncInvokeLog(std::string("") + "音声認識失敗しました。認識:" + " yobikake:" + num2str(yobikakeRuleConfidenceFilter) + " basic:" + num2str(basicRuleConfidenceFilter) + " diccheck:" + num2str((int)dictationCheck) + " dic:" + diction + " failmatch:" + matString ,LOG_LEVEL_DEBUG);
}

//音声認識した結果
void ScriptManager::VoiceRecogntion(const CallbackDataStruct& callback,const std::map< std::string , std::string >& capture,const std::string& diction,double yobikakeRuleConfidenceFilter,double basicRuleConfidenceFilter)
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません
	this->PoolMainWindow->SyncInvokeLog(std::string("") + "音声認識完了しました。認識:" + capture.begin()->second + " diction:" + diction + " yobikake:" + num2str(yobikakeRuleConfidenceFilter) + " basic:" + num2str(basicRuleConfidenceFilter),LOG_LEVEL_DEBUG);
	this->PoolMainWindow->Recognition.ClearTemporary();
	this->fireCallback(callback,capture);
	this->PoolMainWindow->Recognition.CommitRule();
}

//喋り終わった時
void ScriptManager::SpeakEnd(const CallbackDataStruct& callback)
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません
	this->PoolMainWindow->SyncInvokeLog("spack完了しました。コールバックを打ち返します。",LOG_LEVEL_DEBUG);

	std::map<std::string,std::string > dummy;
	this->fireCallback(callback,dummy);
}

//汎用的なコールバック打ち返し
xreturn::r<std::string> ScriptManager::fireCallback(const CallbackDataStruct& callback,const std::map< std::string , std::string >& args) const
{
	int func = callback.getFunc();
	if (func == NO_CALLBACK)
	{
		return "";
	}
	return callback.getRunner()->callbackFunction(func,args);
}



//家電制御が終わった時
void ScriptManager::ActionEnd(const CallbackDataStruct& callback,const std::map< std::string , std::string >& data)
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません
	this->PoolMainWindow->SyncInvokeLog("action完了しました。コールバックを打ち返します。",LOG_LEVEL_DEBUG);

	this->fireCallback(callback,data);
	this->UnrefCallback(callback);
}

//暇な時の動作
void ScriptManager::RegistIdle(const CallbackDataStruct& callback,int wariai)
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません
}

//HTTPで所定のパスにアクセスがあった時
void ScriptManager::HttpRequest(const CallbackDataStruct& callback,const std::string & path ,const std::map< std::string , std::string > & request,std::string * respons,WEBSERVER_RESULT_TYPE* type)
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません

	//webの打ち返しは少々複雑.
	int func = callback.getFunc();
	if (func == NO_CALLBACK)
	{
		*type = WEBSERVER_RESULT_TYPE_NOT_FOUND;
		*respons = "";
		return ;
	}
	*type = WEBSERVER_RESULT_TYPE_OK;
	*respons = callback.getRunner()->callbackWebFunction(func,request,type);
}

//コールバックを消す通知をします。
void ScriptManager::UnrefCallback(const CallbackDataStruct& callback)
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません

	int func = callback.getFunc();
	if (func == NO_CALLBACK)
	{
		return ;
	}
	callback.getRunner()->unrefCallback(func);
}
