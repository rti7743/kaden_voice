#pragma once

#include "common.h"
#include "ScriptRunner.h"


class ScriptManager
{
public:
	ScriptManager()
	{
	}

	virtual ~ScriptManager();
	//作成
	xreturn::r<bool> ScriptManager::Create(MainWindow * poolMainWindow);

	//音声認識に失敗した時の結果
	void ScriptManager::BadVoiceRecogntion(int errorCode,const std::string& matString,const std::string& diction,double yobikakeRuleConfidenceFilter,double basicRuleConfidenceFilter,bool dictationCheck);
	//音声認識した結果
	void ScriptManager::VoiceRecogntion(const CallbackDataStruct* callback,const std::map< std::string , std::string >& capture,const std::string& diction,double yobikakeRuleConfidenceFilter,double basicRuleConfidenceFilter);
	//コールバックを消す通知をします。
	void ScriptManager::UnrefCallback(const CallbackDataStruct* callback);
	//喋り終わった時
	void ScriptManager::SpeakEnd(const CallbackDataStruct* callback,const std::string& text);
	//HTTPで所定のパスにアクセスがあった時
	void ScriptManager::HttpRequest(const CallbackDataStruct* callback,const std::string & path ,const std::map< std::string , std::string > & request,std::string * respons,WEBSERVER_RESULT_TYPE* type,std::string* headers);
	//家電制御が終わった時
	void ScriptManager::ActionEnd(const CallbackDataStruct* callback,const std::map< std::string , std::string >& data);
	//トリガーが呼ばれたとき
	void ScriptManager::TriggerCall(const CallbackDataStruct* callback,const std::map< std::string , std::string >& args,std::string * respons);
	//ウェブメニューからの実行
	void ScriptManager::WebMenuCall(const CallbackDataStruct* callback);
private:

	//luaファイル郡の読み込み
	xreturn::r<bool> ScriptManager::loadLua(const std::string & baseDirectory);
	//汎用的なコールバック打ち返し
	xreturn::r<std::string> ScriptManager::fireCallback(const CallbackDataStruct* callback,const std::map< std::string , std::string >& args) const;
	//luaの終了
	void ScriptManager::destoryLua();
	//luaの実行
	void ScriptManager::RunAllLua();


	MainWindow *              PoolMainWindow;
	std::list<ScriptRunner*> Scripts;
};
