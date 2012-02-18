#pragma once

extern "C" {
	#include "../lua/lua.hpp"
};

enum WEBSERVER_RESULT_TYPE
{
	 WEBSERVER_RESULT_TYPE_OK
	,WEBSERVER_RESULT_TYPE_TRASMITFILE
	,WEBSERVER_RESULT_TYPE_ERROR
	,WEBSERVER_RESULT_TYPE_NOT_FOUND
	,WEBSERVER_RESULT_TYPE_LOCATION
	,WEBSERVER_RESULT_TYPE_FORBIDDEN
};

class ScriptRunner
{
	lua_State* LuaInstance;
	MainWindow * PoolMainWindow;
	bool IsScenario;
	std::string filename;

	//httpd サーバコンテンツを生成する場合の変数.
	std::string WebechoStdout;
	WEBSERVER_RESULT_TYPE WebResultType;


public:
	ScriptRunner( MainWindow * poolMainWindow , bool isScenario)
	{
		this->PoolMainWindow = poolMainWindow;
		this->IsScenario = isScenario;
		this->LuaInstance = NULL;
	}
	virtual ~ScriptRunner()
	{
		if (this->LuaInstance)
		{
			lua_close(this->LuaInstance);
			this->LuaInstance = NULL;
		}
	}

	xreturn::r<bool> LoadScript(const std::string & filename);

	//テーブルからデータを読みます。
	xreturn::r<std::map<std::string,std::string> > ScriptRunner::readLuaTable(const std::string & name,int or_id = 0);
	//グローバルテーブルの中身をすべて読みます
	xreturn::r<std::map<std::string,std::string> > readLuaGlobalTable()
	{
		return readLuaTable("_G" ,0); 
	}

	bool ScriptRunner::IsMethodExist(const std::string& name);
	xreturn::r<std::string> callFunction(const std::string& name);
	xreturn::r<std::string> callFunction(const std::string& name,const std::string& paramA,const std::string& paramB = "",const std::string& paramC = "");
	xreturn::r<std::string> callFunction(const std::string& name,const std::map<std::string , std::string> & match);
	xreturn::r<std::string> callFunction(const std::string& name,const std::list<std::string> & list,bool stripFirst);

	xreturn::r<std::string> callbackFunction(int callbackIndex,const std::map<std::string , std::string> & match);
	xreturn::r<std::string> callbackWebFunction(int callbackIndex,const std::map<std::string , std::string> & match,WEBSERVER_RESULT_TYPE* type);
	//コールバックが不要になった時
	void ScriptRunner::unrefCallback(int func);

private:

	//引数をargsとしてグローバル変数に登録する.
	//void ScriptRunner::RegistArgs(const std::string & filename,const std::vector<std::string>& args);

	static int ScriptRunner::l_onvoice(lua_State* L);
	static int ScriptRunner::l_onvoice_local(lua_State* L);
	static int ScriptRunner::l_onhttp(lua_State* L);
	static int ScriptRunner::l_ontimer(lua_State* L);
	static int ScriptRunner::l_ontrigger(lua_State* L);
	static int ScriptRunner::l_onidle(lua_State* L);
	static int ScriptRunner::l_webreturn(lua_State* L);
	static int ScriptRunner::l_action(lua_State* L);
	static int ScriptRunner::l_speak(lua_State* L);
	static int ScriptRunner::l_set_config(lua_State* L);
	static int ScriptRunner::l_get_config(lua_State* L);
	static int ScriptRunner::l_get_args(lua_State* L);
	static int ScriptRunner::l_get_args_size(lua_State* L);
	static int ScriptRunner::l_telnet(lua_State* L);
	static int ScriptRunner::l_execute(lua_State* L);
	static int ScriptRunner::l_httpget(lua_State* L);
	static int ScriptRunner::l_httppost(lua_State* L);
	static int ScriptRunner::l_sendkey(lua_State* L);
	static int ScriptRunner::l_sendmessage(lua_State* L);
	static int ScriptRunner::l_msleep(lua_State* L);
	static int ScriptRunner::l_dump(lua_State* L);
	static int ScriptRunner::l_callstack(lua_State* L);
	static int ScriptRunner::l_webecho(lua_State* L);
	static int ScriptRunner::l_webload(lua_State* L);
	static int ScriptRunner::l_weblocation(lua_State* L);
	static int ScriptRunner::l_weberror(lua_State* L);
	static int ScriptRunner::l_gotoweb(lua_State* L);
	static int ScriptRunner::l_tips(lua_State* L);


	static ScriptRunner* __this(lua_State* L);
	static std::string ScriptRunner::lua_dump(lua_State* L , int index, int nest);
	static std::string ScriptRunner::lua_callstack(lua_State* L);
	static std::string lua_tostringHelper(lua_State* L , int index);
	static void lua_pushstringHelper(lua_State* L ,const std::string & str);
	static int ScriptRunner::luaL_errorHelper(lua_State* L ,const std::string & errorMessage);
	static xreturn::r<std::string> ScriptRunner::lua_crossdataToString(lua_State* L , int index);
	static std::string ScriptRunner::convertTemplate(const std::string & filename);
	static void ScriptRunner::push_lua_SafeLuaTable(lua_State* L , int index);
	static std::string ScriptRunner::lua_funcdump(lua_State* L ,const std::string& name);

	SEXYTEST_TEST_FRIEND;
};

const int NO_CALLBACK = INT_MAX;

//コールバックするデータを保持する
//コピーされまくるので軽量の構造にするべし.
class CallbackDataStruct
{
public:
	CallbackDataStruct(ScriptRunner* _runner,int _func) : runner(_runner) , func(_func)
	{
#if _DEBUG
		threadid = ::GetCurrentThreadId();
#endif //_DEBUG
	}
	int getFunc() const
	{
#if _DEBUG
		//違うスレッドで実行してはいけない!!
		assert(this->threadid == ::GetCurrentThreadId());
#endif //_DEBUG
		return this->func;
	}
	ScriptRunner* getRunner() const
	{
#if _DEBUG
		//違うスレッドで実行してはいけない!!
		assert(this->threadid == ::GetCurrentThreadId());
#endif //_DEBUG
		return this->runner;
	}

private:
	ScriptRunner* runner;
	int func;
#if _DEBUG
	DWORD threadid;
#endif //_DEBUG
};
