#pragma once

#include "common.h"
#include "ScriptRunner.h"
#include "ScriptWebRunner.h"
#include "XLHttpHeader.h"

class ScriptManager
{
public:
	ScriptManager()
	{
	}

	virtual ~ScriptManager();
	//作成
	bool Create(MainWindow * poolMainWindow);

	//音声認識に失敗した時の結果
	void BadVoiceRecogntion(int errorCode,const std::string& matString,const std::string& diction,double yobikakeRuleConfidenceFilter,double basicRuleConfidenceFilter,bool dictationCheck);
	//音声認識した結果
	void VoiceRecogntion(const CallbackDataStruct* callback,const std::map< std::string , std::string >& capture,const std::string& diction,double yobikakeRuleConfidenceFilter,double basicRuleConfidenceFilter);
	//コールバックを消す通知をします。
	void UnrefCallback(const CallbackDataStruct* callback);
	//喋り終わった時
	void SpeakEnd(const CallbackDataStruct* callback,const std::string& text);
	//家電制御が終わった時
	void ActionEnd(const CallbackDataStruct* callback,const std::map< std::string , std::string >& data);
	//トリガーが呼ばれたとき
	void TriggerCall(const CallbackDataStruct* callback,const std::map< std::string , std::string >& args,std::string * respons);

	void WebMenuCall(const CallbackDataStruct* callback);
	//webからアクセスがあった時
	bool WebAccess(const std::string& path,const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result,std::string* responsString);
private:

	//luaファイル郡の読み込み
	bool loadLua(const std::string & baseDirectory);
	//汎用的なコールバック打ち返し
	std::string fireCallback(const CallbackDataStruct* callback,const std::map< std::string , std::string >& args) const;
	//luaの終了
	void destoryLua();
	//luaの実行
	void RunAllLua();


	MainWindow *              PoolMainWindow;
	std::list<ScriptRunner*>  Scripts;
	ScriptWebRunner			  WebScript;
};
