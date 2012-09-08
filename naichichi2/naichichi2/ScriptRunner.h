#pragma once

extern "C" {
	#include "../lua/lua.hpp"
};
#include "Callbackable.h"

class ScriptRunner : public Callbackable
{
	lua_State* LuaInstance;
	MainWindow * PoolMainWindow;
	bool IsScenario;
	std::string filename;
public:
	ScriptRunner( MainWindow * poolMainWindow , bool isScenario)
	{
		this->PoolMainWindow = poolMainWindow;
		this->IsScenario = isScenario;
		this->LuaInstance = NULL;
	}
	virtual ~ScriptRunner();

	bool LoadScript(const std::string & filename);

	bool IsMethodExist(const std::string& name);
	std::string callFunction(const std::string& name);
	std::string callFunction(const std::string& name,const std::string& paramA,const std::string& paramB = "",const std::string& paramC = "");
	std::string callFunction(const std::string& name,const std::map<std::string , std::string> & match);
	std::string callFunction(const std::string& name,const std::list<std::string> & list,bool stripFirst);

	std::string callbackFunction(const CallbackDataStruct* callback,const std::map<std::string , std::string> & match);
	//コールバックが不要になった時に呼ばれる 自分から this-> で読んではいけない。
	void unrefCallback(const CallbackDataStruct* callback);
	//インスタンスの再読み込み
	//プログラムをデバッグしているときとか、再読み込み機能がないと死ねるから・・・
	bool Reload();

private:

	//新しい luaインスタンスを作成する
	bool CreateLuaInstance();
	//メモリからluaのプログラムを構築(テスト用)
	bool EvalScript(const std::string & script);

	static int l_onvoice(lua_State* L);
	static int l_onvoice_local(lua_State* L);
	static int l_ontimer(lua_State* L);
	static int l_ontrigger(lua_State* L);
	static int l_onidle(lua_State* L);
	static int l_webreturn(lua_State* L);
	static int l_trigger(lua_State* L);
	static int l_action(lua_State* L);
	static int l_speak(lua_State* L);
	static int l_set_config(lua_State* L);
	static int l_get_config(lua_State* L);
	static int l_get_args(lua_State* L);
	static int l_get_args_size(lua_State* L);
	static int l_telnet(lua_State* L);
	static int l_execute(lua_State* L);
	static int l_httpget(lua_State* L);
	static int l_httppost(lua_State* L);
	static int l_sendkey(lua_State* L);
	static int l_sendmessage(lua_State* L);
	static int l_msleep(lua_State* L);
	static int l_dump(lua_State* L);
	static int l_callstack(lua_State* L);
	static int l_gotoweb(lua_State* L);
	static int l_tips(lua_State* L);
	static int l_findmedia(lua_State* L);
	static int l_find_config(lua_State* L);
	static int l_json_encode(lua_State* L);
	static int l_json_decode(lua_State* L);
	static int l_xml_encode(lua_State* L);
	static int l_xml_decode(lua_State* L);
	static int l_getwebmenu(lua_State* L);
	static int l_callwebmenu(lua_State* L);
	static int l_getelectronics(lua_State* L);
	static int l_setelectronics(lua_State* L);



	static ScriptRunner* __this(lua_State* L);
	static std::string lua_dump(lua_State* L , int index,bool win32_nosjisconvert, int nest);
	static std::string lua_callstack(lua_State* L);
	static std::string lua_tostringHelper(lua_State* L , int index);
	static void lua_pushstringHelper(lua_State* L ,const std::string & str);
	static int luaL_errorHelper(lua_State* L ,const std::string & errorMessage);
	static std::string lua_crossdataToString(lua_State* L , int index);
	static std::string convertTemplate(const std::string & filename,const std::string functionname);
	static std::string lua_funcdump(lua_State* L ,const std::string& name);
	//luaのテーブルをjson文字列に変換 文字コードは UTF8のままです。
	static std::string lua_tojson(lua_State* L,int index);

	SEXYTEST_TEST_FRIEND;
};

