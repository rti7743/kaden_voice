#include "common.h"
#include "ScriptRunner.h"
#include "MainWindow.h"
#include "ActionImplement.h"
#include <fstream>
#include "XLStringUtil.h"

ScriptRunner::~ScriptRunner()
{
	if (this->LuaInstance)
	{
		lua_close(this->LuaInstance);
		this->LuaInstance = NULL;
	}
}

//インスタンスの再読み込み
//プログラムをデバッグしているときとか、再読み込み機能がないと死ねるから・・・
xreturn::r<bool> ScriptRunner::Reload()
{
/*
	for(auto it = this->callbackHistoryList.begin() ; it != this->callbackHistoryList.end() ; ++it)
	{
		if (this->PoolMainWindow->Recognition.RemoveCallback(it,false))
		{
			break;	
		}
		if (this->PoolMainWindow->Speak.RemoveCallback(it,false))
		{
			break;	
		}
		if (this->PoolMainWindow->Httpd.RemoveCallback(it,false))
		{
			break;	
		}
	}

	//Luaを飛ばす.
	if (this->LuaInstance)
	{
		lua_close(this->LuaInstance);
		this->LuaInstance = NULL;
	}
*/
	return true;
}

xreturn::r<bool> ScriptRunner::LoadScript(const std::string & filename)
{
	assert(this->LuaInstance == NULL);

	// Luaを開く
	this->filename = filename;

	auto r1 = CreateLuaInstance();
	if (!r1)
	{
		return xreturn::error(std::string("luaインスタンス構築に失敗しました。 Filename:") + this->filename + " Lua:" + r1.getError().getFullErrorMessage() );
	}

	// Luaファイルを読み込む
	if(luaL_loadfile(this->LuaInstance, filename.c_str() ) )
	{
		return xreturn::error(std::string("スクリプト読み込みに失敗しました。 Filename:") + this->filename + " Lua:" + lua_tostringHelper(this->LuaInstance, -1));
	}

	//スクリプトのグローバルなエリアの処理.
	if(lua_pcall(this->LuaInstance, 0, 0, 0) )
	{
		return xreturn::error(std::string("スクリプト読み込み時の実行に失敗しました。 Filename:") + this->filename + " Lua:" + lua_tostringHelper(this->LuaInstance, -1));
	}

	return true;
}


xreturn::r<bool> ScriptRunner::EvalScript(const std::string & script)
{
	assert(this->LuaInstance == NULL);

	// Luaを開く
	this->filename = "<EVAL>";

	auto r1 = CreateLuaInstance();
	if (!r1)
	{
		return xreturn::error(std::string("luaインスタンス構築に失敗しました。 Filename:") + this->filename + " Lua:" + r1.getError().getFullErrorMessage() );
	}

	// Luaファイルを読み込む
	if ( luaL_loadstring(this->LuaInstance, script.c_str()) )
	{
		return xreturn::error(std::string("スクリプト読み込み時に失敗しました。 Filename:") + this->filename + " Lua:" + lua_tostringHelper(this->LuaInstance, -1));
	}

	//スクリプトのグローバルなエリアの処理.
	if(lua_pcall(this->LuaInstance, 0, 0, 0) )
	{
		return xreturn::error(std::string("スクリプト読み込み時の実行に失敗しました。 Filename:") + this->filename + " Lua:" + lua_tostringHelper(this->LuaInstance, -1));
	}

	return true;
}

//新しい luaインスタンスを作成する
xreturn::r<bool> ScriptRunner::CreateLuaInstance()
{
	assert(this->LuaInstance == NULL);

	//新しい luaインスタンスを作成する
	this->LuaInstance = luaL_newstate();

	// Lua標準関数を使う
	luaL_openlibs(this->LuaInstance);
	//ないちちで利用する関数オーバーライド
	lua_register(this->LuaInstance, "onvoice", (lua_CFunction)ScriptRunner::l_onvoice);
	lua_register(this->LuaInstance, "onvoice_local", (lua_CFunction)ScriptRunner::l_onvoice_local);
	lua_register(this->LuaInstance, "onhttp", (lua_CFunction)ScriptRunner::l_onhttp);
	lua_register(this->LuaInstance, "ontrigger", (lua_CFunction)ScriptRunner::l_ontrigger);
	lua_register(this->LuaInstance, "speak", (lua_CFunction)ScriptRunner::l_speak);
	lua_register(this->LuaInstance, "trigger", (lua_CFunction)ScriptRunner::l_trigger);
	lua_register(this->LuaInstance, "action", (lua_CFunction)ScriptRunner::l_action);
	lua_register(this->LuaInstance, "getconfig", (lua_CFunction)ScriptRunner::l_get_config);
	lua_register(this->LuaInstance, "setconfig", (lua_CFunction)ScriptRunner::l_set_config);
	lua_register(this->LuaInstance, "findconfig", (lua_CFunction)ScriptRunner::l_find_config);
	lua_register(this->LuaInstance, "telnet", (lua_CFunction)ScriptRunner::l_telnet);
	lua_register(this->LuaInstance, "execute", (lua_CFunction)ScriptRunner::l_execute);
	lua_register(this->LuaInstance, "httpget", (lua_CFunction)ScriptRunner::l_httpget);
	lua_register(this->LuaInstance, "httppost", (lua_CFunction)ScriptRunner::l_httppost);
	lua_register(this->LuaInstance, "sendkey", (lua_CFunction)ScriptRunner::l_sendkey);
	lua_register(this->LuaInstance, "sendmessage", (lua_CFunction)ScriptRunner::l_sendmessage);
	lua_register(this->LuaInstance, "msleep", (lua_CFunction)ScriptRunner::l_msleep);
	lua_register(this->LuaInstance, "var_dump", (lua_CFunction)ScriptRunner::l_dump);
	lua_register(this->LuaInstance, "dump", (lua_CFunction)ScriptRunner::l_dump);
	lua_register(this->LuaInstance, "print", (lua_CFunction)ScriptRunner::l_dump);
	lua_register(this->LuaInstance, "echo", (lua_CFunction)ScriptRunner::l_dump);
	lua_register(this->LuaInstance, "callstack", (lua_CFunction)ScriptRunner::l_callstack);
	lua_register(this->LuaInstance, "json_encode", (lua_CFunction)ScriptRunner::l_json_encode);
	lua_register(this->LuaInstance, "json_decode", (lua_CFunction)ScriptRunner::l_json_decode);
	lua_register(this->LuaInstance, "xml_encode", (lua_CFunction)ScriptRunner::l_xml_encode);
	lua_register(this->LuaInstance, "xml_decode", (lua_CFunction)ScriptRunner::l_xml_decode);
	lua_register(this->LuaInstance, "gotoweb", (lua_CFunction)ScriptRunner::l_gotoweb);
	lua_register(this->LuaInstance, "tips", (lua_CFunction)ScriptRunner::l_tips);
	lua_register(this->LuaInstance, "findmedia", (lua_CFunction)ScriptRunner::l_findmedia);
	lua_register(this->LuaInstance, "callwebmenu", (lua_CFunction)ScriptRunner::l_callwebmenu);

	//thisの保存
	lua_setusertag(this->LuaInstance,this);

	return true;
}


bool ScriptRunner::IsMethodExist(const std::string& name)
{
	lua_getglobal(this->LuaInstance, name.c_str() );
	bool r = lua_isfunction(this->LuaInstance,-1);
	lua_pop(this->LuaInstance, lua_gettop(this->LuaInstance));

	return r;
}

xreturn::r<std::string> ScriptRunner::callFunction(const std::string& name)
{
	this->PoolMainWindow->SyncInvokeLog(std::string() + "callFunction:" + name + "()",LOG_LEVEL_DEBUG);

	lua_getglobal(this->LuaInstance, name.c_str() );
	if(!lua_isfunction(this->LuaInstance,-1) )
	{
		return xreturn::error(name + " 関数が定義されていません。 Filename:" + this->filename);
	}

	if(lua_pcall(this->LuaInstance, 0, 1, 0) != 0) 
	{
		return xreturn::error(name + "関数呼び出しに失敗しました。 Filename:" + this->filename + " Lua:" + lua_tostringHelper(this->LuaInstance, -1));
	}

	auto value = ScriptRunner::lua_crossdataToString(this->LuaInstance ,-1);
	lua_pop(this->LuaInstance, lua_gettop(this->LuaInstance));
	if (!value)
	{
		return xreturn::error(name + "関数の戻り値が、voidか、数字か、文字列以外のデータになりました。 Filename:" + this->filename );
	}
	return value;
}
xreturn::r<std::string> ScriptRunner::callFunction(const std::string& name,const std::string& paramA,const std::string& paramB,const std::string& paramC)
{
	this->PoolMainWindow->SyncInvokeLog(std::string() + "callFunction:" + name + "(" + paramA + "," + paramB + "," + paramC + ")",LOG_LEVEL_DEBUG);

	lua_getglobal(this->LuaInstance, name.c_str() );
	if(!lua_isfunction(this->LuaInstance,-1) )
	{
		return xreturn::error(name + " 関数が定義されていません。 Filename:" + this->filename);
	}

	lua_pushstringHelper(this->LuaInstance,paramA);
	lua_pushstringHelper(this->LuaInstance,paramB);
	lua_pushstringHelper(this->LuaInstance,paramC);

	if(lua_pcall(this->LuaInstance, 3, 1, 0) != 0) 
	{
		return xreturn::error(name + "関数呼び出しに失敗しました。 Filename:" + this->filename + " Lua:" + lua_tostringHelper(this->LuaInstance, -1));
	}

	auto value = ScriptRunner::lua_crossdataToString(this->LuaInstance ,-1);
	lua_pop(this->LuaInstance, lua_gettop(this->LuaInstance));
	if (!value)
	{
		return xreturn::error(name + "関数の戻り値が、voidか、数字か、文字列以外のデータになりました。 Filename:" + this->filename );
	}
	return value;
}

xreturn::r<std::string> ScriptRunner::callFunction(const std::string& name,const std::map<std::string , std::string> & match)
{
	this->PoolMainWindow->SyncInvokeLog(std::string() + "callFunction:" + name + "(!map!)",LOG_LEVEL_DEBUG);

	lua_getglobal(this->LuaInstance, name.c_str() );
	if(!lua_isfunction(this->LuaInstance,-1) )
	{
		return xreturn::error(name + " 関数が定義されていません。 Filename:" + this->filename);
	}

	//func( { data = value , ... }  ) として呼ぶ.
	lua_newtable(this->LuaInstance);
	for(std::map<std::string , std::string>::const_iterator i =match.begin() ; i != match.end() ; ++i )
	{
		lua_pushstringHelper(this->LuaInstance, i->first.c_str() );
		lua_pushstringHelper(this->LuaInstance, i->second.c_str() );
		lua_settable(this->LuaInstance, -3);
	}

	if(lua_pcall(this->LuaInstance, 1, 1, 0) != 0) 
	{
		return xreturn::error(name + "関数呼び出しに失敗しました。 Filename:" + this->filename + " Lua:" + lua_tostringHelper(this->LuaInstance, -1));
	}

	auto value = ScriptRunner::lua_crossdataToString(this->LuaInstance ,-1);
	lua_pop(this->LuaInstance, lua_gettop(this->LuaInstance));
	if (!value)
	{
		return xreturn::error(name + "関数の戻り値が、voidか、数字か、文字列以外のデータになりました。 Filename:" + this->filename );
	}
	return value;
}

xreturn::r<std::string> ScriptRunner::callFunction(const std::string& name,const std::list<std::string> & list,bool stripFirst)
{
	this->PoolMainWindow->SyncInvokeLog(std::string() + "callFunction:" + name + "(!list!)",LOG_LEVEL_DEBUG);

	lua_getglobal(this->LuaInstance, name.c_str() );
	if(!lua_isfunction(this->LuaInstance,-1) )
	{
		return xreturn::error(name + " 関数が定義されていません。 Filename:" + this->filename);
	}

	int size = list.size();
	auto it = list.begin();
	if (stripFirst)
	{//先頭を削るならば
		it ++;
		size--;
	}
	for( ; it != list.end() ; ++it)
	{
		lua_pushstringHelper(this->LuaInstance, it->c_str() );
	}

	if(lua_pcall(this->LuaInstance, size, 1, 0) != 0) 
	{
		return xreturn::error(name + "関数呼び出しに失敗しました。 Filename:" + this->filename + " Lua:" + lua_tostringHelper(this->LuaInstance, -1));
	}

	auto value = ScriptRunner::lua_crossdataToString(this->LuaInstance ,-1);
	lua_pop(this->LuaInstance, lua_gettop(this->LuaInstance));
	if (!value)
	{
		return xreturn::error(name + "関数の戻り値が、voidか、数字か、文字列以外のデータになりました。 Filename:" + this->filename );
	}
	return value;
}

//コールバックが不要になった時に呼ばれる 自分から this-> で読んではいけない。
void ScriptRunner::unrefCallback(const CallbackDataStruct* callback)
{
	int func = callback->getFunc();
	if (func != NO_CALLBACK)
	{
		luaL_unref(this->LuaInstance,LUA_REGISTRYINDEX, callback->getFunc() );
	}

	Callbackable::unrefCallback(callback);
}

int ScriptRunner::l_onvoice(lua_State* L)
{
	ScriptRunner* _this = __this(L);
	_this->PoolMainWindow->SyncInvokeLog(std::string() + "lua function:" + lua_funcdump(L,"onvoice") ,LOG_LEVEL_DEBUG);

	if (!_this->IsScenario)
	{
		return luaL_errorHelper(L,lua_funcdump(L,"onvoice") + "はシナリオモードのみ有効です");
	}

	//第1引数
	if (! lua_isstring(L,-2) )
	{
		return luaL_errorHelper(L,lua_funcdump(L,"onvoice") + "の第1引数が文字列ではありません");
	}
	std::string p1 = lua_tostringHelper(L,-2);

	//第2引数
	if (! lua_isfunction(L,-1) )
	{
		return luaL_errorHelper(L,lua_funcdump(L,"onvoice") + "の第2引数が関数ではありません");
	}
	int func = luaL_ref(L,LUA_REGISTRYINDEX);

	auto xr = _this->PoolMainWindow->Recognition.AddCommandRegexp( _this->CreateCallback( func ) , p1);
	if(!xr)
	{
		return luaL_errorHelper(L,lua_funcdump(L,"onvoice") + "実行時のエラー:" +xr.getErrorMessage());
	}

	return 0;             //戻り値の数を指定
}

int ScriptRunner::l_onvoice_local(lua_State* L)
{
	ScriptRunner* _this = __this(L);
	_this->PoolMainWindow->SyncInvokeLog(std::string() + "lua function:" + lua_funcdump(L,"onvoice_local") ,LOG_LEVEL_DEBUG);

	if (!_this->IsScenario)
	{
		return luaL_errorHelper(L,lua_funcdump(L,"onvoice_local") + "はシナリオモードのみ有効です");
	}

	//第1引数
	if (! lua_isstring(L,-2) )
	{
		return luaL_errorHelper(L,lua_funcdump(L,"onvoice_local") + "の第1引数が文字列ではありません");
	}
	std::string p1 = lua_tostringHelper(L,-2);

	//第2引数
	if (! lua_isfunction(L,-1) )
	{
		return luaL_errorHelper(L,lua_funcdump(L,"onvoice_local") + "の第2引数が関数ではありません");
	}
	int func = luaL_ref(L,LUA_REGISTRYINDEX);

 
	auto xr = _this->PoolMainWindow->Recognition.AddTemporaryRegexp( _this->CreateCallback( func ) , p1);
	if(!xr)
	{
		return luaL_errorHelper(L,lua_funcdump(L,"onvoice_local") + "実行時のエラー" +xr.getErrorMessage());
	}

	return 0;             //戻り値の数を指定
}



int ScriptRunner::l_onhttp(lua_State* L)
{
	ScriptRunner* _this = __this(L);
	_this->PoolMainWindow->SyncInvokeLog(std::string() + "lua function:" + lua_funcdump(L,"onhttp") ,LOG_LEVEL_DEBUG);

	if (!_this->IsScenario)
	{
		return luaL_errorHelper(L,lua_funcdump(L,"onhttp") + "はシナリオモードのみ有効です");
	}
	int argc = lua_gettop(L);
	std::string key;
	int func;

	if (argc == 2)
	{
		//第1引数
		if (! lua_isstring(L,-2) )
		{
			return luaL_errorHelper(L,lua_funcdump(L,"onhttp") + "の第1引数が文字列ではありません");
		}
		key = lua_tostringHelper(L,-2);

		//第2引数
		if (! lua_isfunction(L,-1) )
		{
			return luaL_errorHelper(L,lua_funcdump(L,"onhttp") + "の第2引数が関数ではありません");
		}
		func = luaL_ref(L,LUA_REGISTRYINDEX);
	}
	else
	{
		return luaL_errorHelper(L,lua_funcdump(L,"onhttp") + "の引数数が正しくありません");
	}

	
	auto xr = _this->PoolMainWindow->Httpd.Regist(_this->CreateCallback( func ) , key);
	if(!xr)
	{
		return luaL_errorHelper(L,lua_funcdump(L,"onhttp") + "実行時のエラー" +xr.getErrorMessage());
	}

	return 0;             //戻り値の数を指定
}

int ScriptRunner::l_ontrigger(lua_State* L)
{
	ScriptRunner* _this = __this(L);
	_this->PoolMainWindow->SyncInvokeLog(std::string() + "lua function:" + lua_funcdump(L,"ontrigger") ,LOG_LEVEL_DEBUG);

	if (!_this->IsScenario)
	{
		return luaL_errorHelper(L,lua_funcdump(L,"ontrigger") + "はシナリオモードのみ有効です");
	}
	int argc = lua_gettop(L);
	std::string key;
	std::string menuname;
	int func;

	if (argc == 3)
	{
		//第1引数
		if (! lua_isstring(L,-3) )
		{
			return luaL_errorHelper(L,lua_funcdump(L,"ontrigger") + "の第1引数が文字列ではありません");
		}
		key = lua_tostringHelper(L,-3);

		//第2引数
		if (! lua_isstring(L,-2) )
		{
			return luaL_errorHelper(L,lua_funcdump(L,"ontrigger") + "の第2引数が文字列ではありません");
		}
		menuname = lua_tostringHelper(L,-2);

		//第3引数
		if (! lua_isfunction(L,-1) )
		{
			return luaL_errorHelper(L,lua_funcdump(L,"ontrigger") + "の第3引数が関数ではありません");
		}
		func = luaL_ref(L,LUA_REGISTRYINDEX);
	}
	else if (argc == 2)
	{
		//第1引数
		if (! lua_isstring(L,-2) )
		{
			return luaL_errorHelper(L,lua_funcdump(L,"ontrigger") + "の第1引数が文字列ではありません");
		}
		key = lua_tostringHelper(L,-2);

		//第2引数
		if (! lua_isfunction(L,-1) )
		{
			return luaL_errorHelper(L,lua_funcdump(L,"ontrigger") + "の第2引数が関数ではありません");
		}
		func = luaL_ref(L,LUA_REGISTRYINDEX);
	}
	else
	{
		return luaL_errorHelper(L,lua_funcdump(L,"ontrigger") + "の引数数が正しくありません");
	}

	
	auto xr = _this->PoolMainWindow->TriggerManager.Regist(_this->CreateCallback( func ) , menuname, key);
	if(!xr)
	{
		return luaL_errorHelper(L,lua_funcdump(L,"ontrigger") + "実行時のエラー" +xr.getErrorMessage());
	}

	return 0;             //戻り値の数を指定
}

int ScriptRunner::l_trigger(lua_State* L)
{
	ScriptRunner* _this = __this(L);
	_this->PoolMainWindow->SyncInvokeLog(std::string() + "lua function:" + lua_funcdump(L,"trigger") ,LOG_LEVEL_DEBUG);

	int argc = lua_gettop(L);
	
	std::string triggername;
	int tableindex ;
	if (argc == 2)
	{
		//第1引数
		if (! lua_isstring(L,-2) )
		{
			return luaL_errorHelper(L,lua_funcdump(L,"trigger") + "の第1引数が文字列ではありません");
		}
		triggername = lua_tostringHelper(L,-2);

		//第2引数
		if (! lua_istable(L,-1) )
		{
			return luaL_errorHelper(L,lua_funcdump(L,"trigger") + "の第2引数がテーブルではありません");
		}
		tableindex = -1;
	}
	else
	{
		return luaL_errorHelper(L,lua_funcdump(L,"trigger") + "の引数数が正しくありません。");
	}

	if (! _this->PoolMainWindow->TriggerManager.IsExist(triggername) )
	{
		return luaL_errorHelper(L,lua_funcdump(L,"trigger") + "指定されたトリガー " + triggername + "は存在しません");
	}


	std::map<std::string,std::string> args;

	lua_pushnil(L);
	while (lua_next(L, tableindex - 1) != 0) 
	{
		if (lua_istable(L, -1)) 
		{
			return luaL_errorHelper(L,lua_funcdump(L,"trigger") + " ネストしたテーブルは利用できません。1次元テーブルだけにしてください。");
		}

		args.insert( std::pair<std::string,std::string>(lua_crossdataToString(L,-2),lua_crossdataToString(L,-1)) );

		lua_pop(L, 1);
	}

	auto r1 = _this->PoolMainWindow->TriggerManager.Call(triggername , args);
	if (!r1)
	{
		return luaL_errorHelper(L,lua_funcdump(L,"trigger") + " 実行中にエラーが発生しました。" + r1.getFullErrorMessage() );
	}

	lua_pushstringHelper(L,r1);

	return 1;             //戻り値の数を指定
}
int ScriptRunner::l_action(lua_State* L)
{
	ScriptRunner* _this = __this(L);
	_this->PoolMainWindow->SyncInvokeLog(std::string() + "lua function:" + lua_funcdump(L,"action") ,LOG_LEVEL_DEBUG);

	if (!_this->IsScenario)
	{
		return luaL_errorHelper(L,lua_funcdump(L,"action") + "はシナリオモードのみ有効です");
	}
	int argc = lua_gettop(L);
	
	std::string action;
	int func;
	if (argc == 1)
	{
		//第1引数
		if (! lua_isstring(L,-1) )
		{
			return luaL_errorHelper(L,lua_funcdump(L,"action") + "の第1引数が文字列ではありません");
		}
		action = lua_tostringHelper(L,-1);
		//第二引数省略
		func = NO_CALLBACK;
	}
	else if (argc == 2)
	{
		//第1引数
		if (! lua_isstring(L,-2) )
		{
			return luaL_errorHelper(L,lua_funcdump(L,"action") + "の第1引数が文字列ではありません");
		}
		action = lua_tostringHelper(L,-2);

		//第2引数
		if (! lua_isfunction(L,-1) )
		{
			return luaL_errorHelper(L,lua_funcdump(L,"action") + "の第2引数が関数ではありません");
		}
		func = luaL_ref(L,LUA_REGISTRYINDEX);
	}
	else
	{
		return luaL_errorHelper(L,lua_funcdump(L,"action") + "の引数数が正しくありません。");
	}

	auto xr = _this->PoolMainWindow->WebMenu.Fire(_this->CreateCallback( func ) ,_this->PoolMainWindow->WebMenu.getRoomName(), action);
	if(!xr)
	{
		return luaL_errorHelper(L,lua_funcdump(L,"action") + "実行時のエラー:" + xr.getErrorMessage());
	}

	return 0;             //戻り値の数を指定
}

int ScriptRunner::l_speak(lua_State* L)
{
	ScriptRunner* _this = __this(L);
	_this->PoolMainWindow->SyncInvokeLog(std::string() + "lua function:" + lua_funcdump(L,"speak") ,LOG_LEVEL_DEBUG);

	if (!_this->IsScenario)
	{
		return luaL_errorHelper(L,lua_funcdump(L,"speak") + "はシナリオモードのみ有効です");
	}
	int argc = lua_gettop(L);
	
	std::string text;
	int func;
	if (argc == 1)
	{
		//第1引数
		if (! lua_isstring(L,-1) )
		{
			return luaL_errorHelper(L,lua_funcdump(L,"speak") + "の第1引数が文字列ではありません");
		}
		text = lua_tostringHelper(L,-1);
		//第二引数省略
		func = NO_CALLBACK;
	}
	else if (argc == 2)
	{
		//第1引数
		if (! lua_isstring(L,-2) )
		{
			return luaL_errorHelper(L,lua_funcdump(L,"speak") + "の第1引数が文字列ではありません");
		}
		text = lua_tostringHelper(L,-2);

		//第2引数
		if (! lua_isfunction(L,-1) )
		{
			return luaL_errorHelper(L,lua_funcdump(L,"speak") + "の第2引数が関数ではありません");
		}
		func = luaL_ref(L,LUA_REGISTRYINDEX);
	}
	else
	{
		return luaL_errorHelper(L,lua_funcdump(L,"speak") + "の引数数が正しくありません。");
	}

	auto xr = _this->PoolMainWindow->Speak.Speak(_this->CreateCallback( func ) ,text);
	if(!xr)
	{
		return luaL_errorHelper(L,lua_funcdump(L,"speak") + "実行時のエラー:" +xr.getErrorMessage());
	}

	return 0;             //戻り値の数を指定
}


int ScriptRunner::l_set_config(lua_State* L)
{
	ScriptRunner* _this = __this(L);
	_this->PoolMainWindow->SyncInvokeLog(std::string() + "lua function:" + lua_funcdump(L,"set_config") ,LOG_LEVEL_DEBUG);

	//キー
	if (! lua_isstring(L,-2) )
	{
		return luaL_errorHelper(L,lua_funcdump(L,"set_config") + "の第1引数が文字列ではありません");
	}
	std::string key = lua_tostringHelper(L,-2);

	//値
	auto value = ScriptRunner::lua_crossdataToString(L ,-1);
	if (!value)
	{
		return luaL_errorHelper(L,lua_funcdump(L,"set_config") + "の第2引数が文字列または数字ではありません");
	}

	_this->PoolMainWindow->Config.Set(key,value);
	return 0;
}

int ScriptRunner::l_get_config(lua_State* L)
{
	ScriptRunner* _this = __this(L);
	_this->PoolMainWindow->SyncInvokeLog(std::string() + "lua function:" + lua_funcdump(L,"get_config") ,LOG_LEVEL_DEBUG);

	std::string key;
	std::string def;

	int argc = lua_gettop(L);
	if (argc == 2)
	{
		//第一引数 キー
		if (! lua_isstring(L,-2) )
		{
			return luaL_errorHelper(L,lua_funcdump(L,"get_config") + "の第1引数が文字列ではありません");
		}
		key = lua_tostringHelper(L,-2);

		//第二引数 ディフォルト
		auto value = ScriptRunner::lua_crossdataToString(L ,-1);
		if (!value)
		{
			return luaL_errorHelper(L,lua_funcdump(L,"get_config") + "の第2引数が文字列または数字ではありません");
		}
		def = value;
	}
	else if (argc == 1)
	{
		//キー
		if (! lua_isstring(L,-1) )
		{
			return luaL_errorHelper(L,lua_funcdump(L,"get_config") + "の第1引数が文字列ではありません");
		}
		key = lua_tostringHelper(L,-1);
	}

	//キーの値を戻り値に積み増す.
	std::string value = _this->PoolMainWindow->Config.Get(key,def);
	lua_pushstringHelper(L,value);
	return 1;
}

int ScriptRunner::l_telnet(lua_State* L)
{
	ScriptRunner* _this = __this(L);
	_this->PoolMainWindow->SyncInvokeLog(std::string() + "lua function:" + lua_funcdump(L,"telnet") ,LOG_LEVEL_DEBUG);

	std::string ret;

	int argc = lua_gettop(L);
	if (argc != 5)
	{
		return luaL_errorHelper(L,lua_funcdump(L,"telnet") + "の引数が正しくありません。");
	}
	//第一引数 host
	if (! lua_isstring(L,-5) )
	{
		return luaL_errorHelper(L,lua_funcdump(L,"telnet") + "の第1引数が文字列ではありません");
	}
	std::string host= lua_tostringHelper(L,-5);

	//第ニ引数 port
	auto portString = ScriptRunner::lua_crossdataToString(L ,-4);
	if (!portString)
	{
		return luaL_errorHelper(L,lua_funcdump(L,"telnet") + "の第2引数が数字ではありません");
	}
	int port = atoi( ((std::string)portString).c_str() );

	//第三引数 wait
	if (! lua_isstring(L,-3) )
	{
		return luaL_errorHelper(L,lua_funcdump(L,"telnet") + "の第3引数が文字列ではありません");
	}
	std::string wait = lua_tostringHelper(L,-3);

	//第四引数 send
	if (! lua_isstring(L,-2) )
	{
		return luaL_errorHelper(L,lua_funcdump(L,"telnet") + "の第4引数が文字列ではありません");
	}
	std::string send = lua_tostringHelper(L,-2);

	//第五引数 recv
	if (! lua_isstring(L,-1) )
	{
		return luaL_errorHelper(L,lua_funcdump(L,"telnet") + "の第5引数が文字列ではありません");
	}
	std::string recv = lua_tostringHelper(L,-1);

	auto r = ActionImplement::Telnet(host , (int)port , wait,send,recv);
	if (!r)
	{
		return luaL_errorHelper(L,lua_funcdump(L,"telnet") + "がエラーになりました。 " + r.getFullErrorMessage() );
	}

	//戻り値を積む.
	lua_pushstringHelper(L,r);
	return 1;
}
int ScriptRunner::l_httpget(lua_State* L)
{
	ScriptRunner* _this = __this(L);
	_this->PoolMainWindow->SyncInvokeLog(std::string() + "lua function:" + lua_funcdump(L,"httpget") ,LOG_LEVEL_DEBUG);

	std::string ret;

	//第一引数 url
	if (! lua_isstring(L,-1) )
	{
		return luaL_errorHelper(L,lua_funcdump(L,"httpget") + "の第1引数が文字列ではありません");
	}
	std::string url= lua_tostringHelper(L,-1);

	auto r = ActionImplement::HttpGet(url);
	if (!r)
	{
		return luaL_errorHelper(L,lua_funcdump(L,"httpget") + "がエラーになりました。 " + r.getFullErrorMessage() );
	}

	//戻り値を積む.
	lua_pushstringHelper(L,r);
	return 1;
}

int ScriptRunner::l_httppost(lua_State* L)
{
	ScriptRunner* _this = __this(L);
	_this->PoolMainWindow->SyncInvokeLog(std::string() + "lua function:" + lua_funcdump(L,"httppost") ,LOG_LEVEL_DEBUG);

	std::string ret;

	//第一引数 url
	if (! lua_isstring(L,-1) )
	{
		return luaL_errorHelper(L,lua_funcdump(L,"httppost") + "の第1引数が文字列ではありません");
	}
	std::string url= lua_tostringHelper(L,-1);

	if (! lua_isstring(L,-2) )
	{
		return luaL_errorHelper(L,lua_funcdump(L,"httppost") + "の第2引数が文字列ではありません");
	}
	std::string data= lua_tostringHelper(L,-2);

	auto r = ActionImplement::HttpPost(url,data);
	if (!r)
	{
		return luaL_errorHelper(L,lua_funcdump(L,"httppost") + "がエラーになりました。 " + r.getFullErrorMessage() );
	}

	//戻り値を積む.
	lua_pushstringHelper(L,r);
	return 1;
}

int ScriptRunner::l_execute(lua_State* L)
{
	ScriptRunner* _this = __this(L);
	_this->PoolMainWindow->SyncInvokeLog(std::string() + "lua function:" + lua_funcdump(L,"execute") ,LOG_LEVEL_DEBUG);

	std::string ret;

	//第1引数 
	if (! lua_isstring(L,-2) )
	{
		return luaL_errorHelper(L,lua_funcdump(L,"execute") + "の第1引数が文字列ではありません");
	}
	std::string command= lua_tostringHelper(L,-2);

	//第2引数 
	if (! lua_isstring(L,-1) )
	{
		return luaL_errorHelper(L,lua_funcdump(L,"execute") + "の第2引数が文字列ではありません");
	}
	std::string args= lua_tostringHelper(L,-1);
	//ベースディレクトリ
	std::string baseDirectory = _this->PoolMainWindow->Config.GetBaseDirectory();

	if ( XLStringUtil::strpos(command,"http://") == 0 || XLStringUtil::strpos(command,"https://") == 0 )
	{
		ActionImplement::OpenWeb("",command);
		return 0;
	}

	//拡張子
	const std::string ext = XLStringUtil::strtolower(XLStringUtil::baseext_nodot( command ));
	//動作を変更する拡張子はあるかな？ めったに使わないので、これでも速度に影響はないだろう。
	const std::string overraideCommand =_this->PoolMainWindow->Config.Get(std::string("action__executeext_") + ext, "");
	if (! overraideCommand.empty() )
	{//あるので動作を上書きする.
		args = XLStringUtil::chop( XLStringUtil::escapeshellarg(command) + " " + args );
		command = overraideCommand;
	}
	command = XLStringUtil::pathcombine(baseDirectory,command);

	auto r = ActionImplement::Execute("",command,args);
	if (!r)
	{
		return luaL_errorHelper(L,lua_funcdump(L,"execute") + "がエラーになりました。 " + r.getFullErrorMessage() );
	}

	return 0;
}

int ScriptRunner::l_sendkey(lua_State* L)
{
	ScriptRunner* _this = __this(L);
	_this->PoolMainWindow->SyncInvokeLog(std::string() + "lua function:" + lua_funcdump(L,"sendkey") ,LOG_LEVEL_DEBUG);

	std::string ret;

	//第1引数 
	if (! lua_isstring(L,-3) )
	{
		return luaL_errorHelper(L,lua_funcdump(L,"sendkey") + "の第1引数が文字列ではありません");
	}
	std::string windowname= lua_tostringHelper(L,-3);

	//第2引数 
	if (! lua_isnumber(L,-2) )
	{
		return luaL_errorHelper(L,lua_funcdump(L,"sendkey") + "の第2引数が数字ではありません");
	}
	int key = lua_tointeger(L,-2);

	//第3引数 
	if (! lua_isnumber(L,-1) )
	{
		return luaL_errorHelper(L,lua_funcdump(L,"sendkey") + "の第3引数が数字ではありません");
	}
	int keyoption = lua_tointeger(L,-1);

	auto r = ActionImplement::SendKeydown("",windowname,key,keyoption);
	if (!r)
	{
		return luaL_errorHelper(L,lua_funcdump(L,"sendkey") + "がエラーになりました。 " + r.getFullErrorMessage() );
	}

	return 0;
}

int ScriptRunner::l_sendmessage(lua_State* L)
{
	ScriptRunner* _this = __this(L);
	_this->PoolMainWindow->SyncInvokeLog(std::string() + "lua function:" + lua_funcdump(L,"sendmessage") ,LOG_LEVEL_DEBUG);

	std::string ret;

	//第1引数 
	if (! lua_isstring(L,-4) )
	{
		return luaL_errorHelper(L,lua_funcdump(L,"sendmessage") + "の第1引数が文字列ではありません");
	}
	std::string windowname= lua_tostringHelper(L,-4);

	//第2引数 
	if (! lua_isnumber(L,-3) )
	{
		return luaL_errorHelper(L,lua_funcdump(L,"sendmessage") + "の第2引数が数字ではありません");
	}
	int message = lua_tointeger(L,-3);

	//第3引数 
	if (! lua_isnumber(L,-2) )
	{
		return luaL_errorHelper(L,lua_funcdump(L,"sendmessage") + "の第3引数が数字ではありません");
	}
	int wparam = lua_tointeger(L,-2);

	//第4引数 
	if (! lua_isnumber(L,-1) )
	{
		return luaL_errorHelper(L,lua_funcdump(L,"sendmessage") + "の第4引数が数字ではありません");
	}
	int lparam = lua_tointeger(L,-1);

	auto r = ActionImplement::SendMessage("",windowname,message,wparam,lparam);
	if (!r)
	{
		return luaL_errorHelper(L,lua_funcdump(L,"sendmessage") + "がエラーになりました。 " + r.getFullErrorMessage() );
	}

	return 0;
}

int ScriptRunner::l_msleep(lua_State* L)
{
	ScriptRunner* _this = __this(L);
	_this->PoolMainWindow->SyncInvokeLog(std::string() + "lua function:" + lua_funcdump(L,"msleep") ,LOG_LEVEL_DEBUG);

	std::string ret;

	//第1引数 寝る時間
	if (! lua_isnumber(L,-1) )
	{
		return luaL_errorHelper(L,lua_funcdump(L,"msleep") + "の第1引数が数字ではありません");
	}
	int mtime = lua_tointeger(L,-1);
	if (mtime <= 0)
	{
		return luaL_errorHelper(L,lua_funcdump(L,"msleep") + "の第1引数が0以下です。スリープするミリ秒を入れてください。");
	}
	auto r = ActionImplement::MSleep("" , (unsigned int)mtime);
	if (!r)
	{
		return luaL_errorHelper(L,lua_funcdump(L,"msleep") + "がエラーになりました。 " + r.getFullErrorMessage() );
	}
	return 0;
}

int ScriptRunner::l_dump(lua_State* L)
{
	ScriptRunner* _this = __this(L);

	int argc = lua_gettop(L) * -1;
	for(;argc < 0;argc++)
	{
		_this->PoolMainWindow->SyncInvokeLog( lua_dump(L , argc,false,0) ,LOG_LEVEL_NOTIFY);
	}

	return 0;
}



//続きはwebで
int ScriptRunner::l_gotoweb(lua_State* L)
{
	ScriptRunner* _this = __this(L);
	_this->PoolMainWindow->SyncInvokeLog(std::string() + "lua function:" + lua_funcdump(L,"gotoweb") ,LOG_LEVEL_DEBUG);

	//第一引数 webpath
	auto webpath = ScriptRunner::lua_crossdataToString(L ,-1);
	if (!webpath)
	{
		return luaL_errorHelper(L,lua_funcdump(L,"gotoweb") + "の第1引数が数字ではありません");
	}

	std::string weburl = _this->PoolMainWindow->Httpd.getWebURL(webpath);

	//戻り値として、weburlを返す.
	lua_pushstringHelper(L,weburl);
	return 1;
}

//バルーンヘルプを出す
int ScriptRunner::l_tips(lua_State* L)
{
	ScriptRunner* _this = __this(L);
	_this->PoolMainWindow->SyncInvokeLog(std::string() + "lua function:" + lua_funcdump(L,"tips") ,LOG_LEVEL_DEBUG);

	std::string title;
	std::string message;

	int argc = lua_gettop(L);
	if (argc <= 0)
	{
		return luaL_errorHelper(L,lua_funcdump(L,"tips") + "の第1引数が文字列ではありません");
	}
	else if (argc == 1)
	{
		title = "";
		auto r1 = ScriptRunner::lua_crossdataToString(L ,-1);
		if (!r1)
		{
			return luaL_errorHelper(L,lua_funcdump(L,"tips") + "の第1引数が文字列ではありません");
		}
		message = r1;
	}
	else 
	{
		auto r1 = ScriptRunner::lua_crossdataToString(L ,-2);
		if (!r1)
		{
			return luaL_errorHelper(L,lua_funcdump(L,"tips") + "の第1引数が文字列ではありません");
		}
		title = r1;

		auto r2 = ScriptRunner::lua_crossdataToString(L ,-1);
		if (!r1)
		{
			return luaL_errorHelper(L,lua_funcdump(L,"tips") + "の第2引数が文字列ではありません");
		}
		message = r2;
	}

	_this->PoolMainWindow->SyncInvokePopupMessage(title,message);

	return 0;
}

//メディアから検索
int ScriptRunner::l_findmedia(lua_State* L)
{
	return 0;
}


int ScriptRunner::l_find_config(lua_State* L)
{
	ScriptRunner* _this = __this(L);
	_this->PoolMainWindow->SyncInvokeLog(std::string() + "lua function:" + lua_funcdump(L,"findconfig") ,LOG_LEVEL_DEBUG);

	//キー
	auto key = ScriptRunner::lua_crossdataToString(L ,-1);
	if (!key)
	{
		return luaL_errorHelper(L,lua_funcdump(L,"findconfig") + "の第1引数が文字列ではありません");
	}

	auto configmap = _this->PoolMainWindow->Config.FindGetsToMap(key,true);

	//lua配列として積み直す
	lua_newtable(L);
	for(auto it = configmap.begin() ; it != configmap.end() ; ++it )
	{
		lua_pushstringHelper(L, it->first );
		lua_pushstringHelper(L, it->second );
		lua_settable(L, -3);
	}
	return 1;
}


int ScriptRunner::l_callstack(lua_State* L)
{
	ScriptRunner* _this = __this(L);

	_this->PoolMainWindow->SyncInvokeLog( lua_callstack(L) );

	return 0;
}

std::string ScriptRunner::convertTemplate(const std::string & filename,const std::string functionname)
{
	enum TPL_STATE
	{
		 TPL_STATE_HTML
		,TPL_STATE_CODE
		,TPL_STATE_DOUBLE_QUOTE
		,TPL_STATE_SINGLE_QUOTE
	};
	TPL_STATE state = TPL_STATE_HTML;

	std::ostringstream out;
	out << "function " << functionname << "(out) ";
	out << "webecho( [[";

	const char * lineCommentStart = NULL;
	bool easy_echo = false;
	std::ifstream TPL( filename );
	std::string line;
	while( std::getline(TPL,line) )
	{
		lineCommentStart = NULL;

		const char * p = line.c_str();
		const char * start = p;
		for( ; *p ; ++p )
		{
			if (state == TPL_STATE_HTML)
			{
				if (*p == '<'  && *(p+1) == '?')
				{
					if (*(p+2) == '=')
					{
						//<?=
						state = TPL_STATE_CODE;
						out << std::string(start , 0 , (int)(p - start) )  << "]] ); webecho(";
						start = p + 2 + 1;
						p+=2;
						easy_echo = true;
						lineCommentStart = NULL;
					}
					else if (*(p+2) == 'l' && *(p+3) == 'u' && *(p+4) == 'a')
					{
						//<?lua
						state = TPL_STATE_CODE;
						out << std::string(start , 0 , (int)(p - start) )  << "]] ); ";
						start = p + 4 + 1;
						p+=4;
						easy_echo = false;
						lineCommentStart = NULL;
					}
				}
			}
			else if (state == TPL_STATE_CODE)
			{
				if (*p == '\\' && *(p+1) == '"')
				{
					p++;	//skip
				}
				else if (*p == '\\' && *(p+1) == '\'')
				{
					p++;	//skip
				}
				else if ( (*p == '-' && *(p+1) == '-') || (*p == '/' && *(p+1) == '/') )
				{
					if (lineCommentStart == NULL)
					{
						lineCommentStart = p;
					}
				}
				else if (*p == '"')
				{
					state = TPL_STATE_DOUBLE_QUOTE;
				}
				else if (*p == '\'')
				{
					state = TPL_STATE_SINGLE_QUOTE;
				}
				else if (*p == '?' && *(p+1) == '>')
				{
					state = TPL_STATE_HTML;
					const char * codeend = p;

					//今の行にシングルコメントが入っている場合は、そのコメントの前まで。
					if ( lineCommentStart )
					{
						codeend = lineCommentStart;
					}

					if ( easy_echo  )
					{
						out << std::string(start , 0 , (int)(codeend - start) )  << "); webecho( [[";
					}
					else 
					{
						out << std::string(start , 0 , (int)(codeend - start) )  << "; webecho( [[";
					}
					start = p + 2 ;
					p ++;
				}
			}
			else if (state == TPL_STATE_DOUBLE_QUOTE)
			{
				if (*p == '\\' && *(p+1) == '"')
				{
					p++;	//skip
				}
				else if (*p == '"')
				{
					state = TPL_STATE_CODE;
				}
			}
			else if (state == TPL_STATE_DOUBLE_QUOTE)
			{
				if (*p == '\\' && *(p+1) == '\'')
				{
					p++;	//skip
				}
				else if (*p == '\'')
				{
					state = TPL_STATE_CODE;
				}
			}
		}
		out << std::string(start , 0 , (int)(p - start) ) << std::endl;
	}
	if(state == TPL_STATE_HTML)
	{
		out << "]] );";
	}
	out << " end";
	//空出力を削除してパフォーマンスを上げる
	std::string sourceCode = out.str();
	sourceCode = XLStringUtil::replace(sourceCode,"webecho( [[]] );","");
	sourceCode = XLStringUtil::replace(sourceCode,"webecho( [[\r\n]] );","");
	return sourceCode;
}

xreturn::r<std::string> ScriptRunner::callbackFunction(const CallbackDataStruct* callback,const std::map<std::string , std::string> & match)
{
	const int callbackIndex = callback->getFunc();
	this->PoolMainWindow->SyncInvokeLog(std::string() + "callbackFunction:" + num2str(callbackIndex) ,LOG_LEVEL_DEBUG);

	//LUA_REGISTRYINDEX に預けておいたコールバックポイントの復元
	lua_rawgeti(this->LuaInstance,LUA_REGISTRYINDEX, callbackIndex);

	//func( { data = value , ... }  ) として呼ぶ.
	lua_newtable(this->LuaInstance);
	for(std::map<std::string , std::string>::const_iterator i =match.begin() ; i != match.end() ; ++i )
	{
		this->PoolMainWindow->SyncInvokeLog(std::string() + " " + i->first + "=>"  + i->second ,LOG_LEVEL_DEBUG);
		lua_pushstringHelper(this->LuaInstance, i->first.c_str() );
		lua_pushstringHelper(this->LuaInstance, i->second.c_str() );
		lua_settable(this->LuaInstance, -3);
	}

	if(lua_pcall(this->LuaInstance, 1, 1, 0) != 0) 
	{
		return xreturn::error(("戻り値のcallback関数呼び出しに失敗しました  Filename:") + this->filename + " Lua:" +  lua_tostringHelper(this->LuaInstance, -1));
	}


	auto value = ScriptRunner::lua_crossdataToString(this->LuaInstance ,-1);
	lua_pop(this->LuaInstance, lua_gettop(this->LuaInstance));
	if (!value)
	{
		return xreturn::error(std::string("callback関数 の戻り値が、voidか、数字か、文字列以外のデータになりました。 Filename:") + this->filename );
	}

	return value;
}



std::string ScriptRunner::lua_tostringHelper(lua_State* L , int index)
{
	_USE_WINDOWS_ENCODING;

#ifdef _WINDOWS
	return _U2A(lua_tostring(L, index));
#else
	return lua_tostring(L, index);
#endif
}

void ScriptRunner::lua_pushstringHelper(lua_State* L ,const std::string & str)
{
	_USE_WINDOWS_ENCODING;

#ifdef _WINDOWS
	lua_pushstring(L,_A2U(str.c_str()));
#else
	lua_pushstring(L,str.c_str());
#endif
}

int ScriptRunner::luaL_errorHelper(lua_State* L ,const std::string & errorMessage)
{
	_USE_WINDOWS_ENCODING;

#ifdef _WINDOWS
	return luaL_error(L,_A2U(errorMessage.c_str()));
#else
	return luaL_error(L,errorMessage.c_str());
#endif
}


xreturn::r<std::string> ScriptRunner::lua_crossdataToString(lua_State* L , int index)
{
	if(lua_isstring(L,index)) 
	{//文字列
		return lua_tostringHelper(L,index);
	}
	else if(lua_isnumber(L,index)) 
	{//数字
		return num2str( lua_tonumber(L,index) );
	}
	else if (lua_isboolean(L,index) )
	{//boolean
		return num2str( lua_toboolean(L,index) );
	}
	else if (lua_isnil(L,index) )
	{//nil
//		return "";
		return std::string("");
	}
	else 
	{
		return xreturn::error("文字列または数字ではありません");
	}
}

std::string ScriptRunner::lua_dump(lua_State* L , int index,bool win32_nosjisconvert, int nest)
{
	_USE_WINDOWS_ENCODING;

	if(lua_isstring(L,index)) 
	{//文字列
#ifdef _WINDOWS
		return win32_nosjisconvert ? lua_tostring(L, index) : _U2A(lua_tostring(L, index));
#else
		return lua_tostring(L, index);
#endif
	}
	else if(lua_isnumber(L,index)) 
	{//数字
		return num2str( lua_tonumber(L,index) );
	}
	else if (lua_isboolean(L,index) )
	{//boolean
		return num2str( lua_toboolean(L,index) );
	}
	else if (lua_isnil(L,index) )
	{//nil
		return "";
	}
	else if(lua_istable(L, index)) 
	{
		std::string out = "{\r\n";
		std::string nest_space(nest * 4, ' ');
		lua_pushnil(L);
		while (lua_next(L, index - 1) != 0) 
		{
#ifdef _WINDOWS
			std::string key =  win32_nosjisconvert ? 
				lua_tostring(L, -2) : _U2A(lua_tostring(L, -2));
#else
			std::string key = lua_tostring(L, -2);
#endif
			out += nest_space + key + ":" + lua_dump(L,-1 ,win32_nosjisconvert, nest + 1) + "\r\n";

			lua_pop(L, 1);
		}
		out += nest_space + "}\r\n";
		return out;
	}
#ifdef _WINDOWS
	return win32_nosjisconvert ? lua_typename(L, lua_type(L, index)) : _A2U(lua_typename(L, lua_type(L, index)));
#else
	return lua_typename(L, lua_type(L, index));
#endif
}

int ScriptRunner::l_json_encode(lua_State* L)
{
	ScriptRunner* _this = __this(L);

	if (lua_istable(L,-1))
	{
		return luaL_errorHelper(L,lua_funcdump(L,"l_json_encode") + "の第1引数がテーブルではありません");
	}
	lua_pushstring(L,  lua_tojson(L,-1).c_str() );
	return 1;
}

int ScriptRunner::l_json_decode(lua_State* L)
{
	return 0;
}


int ScriptRunner::l_xml_encode(lua_State* L)
{
	return 0;
}
int ScriptRunner::l_xml_decode(lua_State* L)
{
	return 0;
}




int ScriptRunner::l_callwebmenu(lua_State* L)
{
	ScriptRunner* _this = __this(L);
	_this->PoolMainWindow->SyncInvokeLog(std::string() + "lua function:" + lua_funcdump(L,"callwebmenu") ,LOG_LEVEL_DEBUG);

	//第1引数
	if (! lua_isstring(L,-3) )
	{
		return luaL_errorHelper(L,lua_funcdump(L,"callwebmenu") + "の第1引数が文字列ではありません");
	}
	std::string roomname = lua_tostringHelper(L,-3);

	//第2引数
	if (! lua_isstring(L,-2) )
	{
		return luaL_errorHelper(L,lua_funcdump(L,"callwebmenu") + "の第2引数が文字列ではありません");
	}
	std::string menuname = lua_tostringHelper(L,-2);

	//第3引数
	if (! lua_isstring(L,-1) )
	{
		return luaL_errorHelper(L,lua_funcdump(L,"callwebmenu") + "の第3引数が文字列ではありません");
	}
	std::string actionanme = lua_tostringHelper(L,-1);

	_this->PoolMainWindow->WebMenu.Fire(_this->CreateCallback( NO_CALLBACK ) ,roomname,menuname,actionanme);
	return 0;             //戻り値の数を指定
}


std::string ScriptRunner::lua_callstack(lua_State* L)
{
	//http://zeuxcg.org/2010/11/07/lua-callstack-with-c-debugger/
	_USE_WINDOWS_ENCODING;
	lua_Debug entry;

	std::stringstream out;
	for (int depth = 0; lua_getstack(L, depth, &entry) ; ++depth)
    {
        int status = lua_getinfo(L, "Sln", &entry);
        assert(status);
 
		out << entry.short_src << "(" << entry.currentline << ")" << (entry.name ? entry.name : "?") << std::endl;
    }
	
#ifdef _WINDOWS
	return _U2A(out.str().c_str());
#else
	return out.str();
#endif
}
std::string ScriptRunner::lua_funcdump(lua_State* L ,const std::string& name)
{
	std::string r = name + "(";
	int i = lua_gettop(L) * -1;
	for( ; i < 0 ; ++i)
	{
		r += "(" + lua_dump(L,i , false,0) + ")";
		if (i != -1) r += "  ,";
	}
	return r + ")";
}

//luaのテーブルをjson文字列に変換 文字コードは UTF8のままです。
std::string ScriptRunner::lua_tojson(lua_State* L,int index)
{
	struct _jsonloop
	{
		lua_State* L;
		std::ostringstream out;
		_jsonloop(lua_State* l) : L(l) 
		{
		}
		void loop(int index)
		{
			if(lua_isstring(L,index)) 
			{//文字列
				this->out << "\"" << XLStringUtil::htmlspecialchars_low(lua_tostring(L, index)) << "\"";
			}
			else if(lua_isnumber(L,index)) 
			{//数字
				this->out << lua_tonumber(L,index) ;
			}
			else if (lua_isboolean(L,index) )
			{//boolean
				this->out << lua_toboolean(L,index) ? "true" : "false";
			}
			else if (lua_isnil(L,index) )
			{//nil
				this->out << "null";
			}
			else if(lua_istable(L, index)) 
			{
				this->out << "{" << std::endl;
				lua_pushnil(L);
				if (lua_next(L, index - 1))
				{
					this->out << "\"" << XLStringUtil::htmlspecialchars_low( lua_tostring(L, -2) ) << "\""; //UTF-8なので low の方を使います
					this->out << ":";
					loop(-1);
					lua_pop(L, 1);

					while (lua_next(L, index - 1) != 0) 
					{
						this->out << ",";
						this->out << "\"" << XLStringUtil::htmlspecialchars_low( lua_tostring(L, -2) ) << "\""; //UTF-8なので low の方を使います
						this->out << ":";
						loop(-1);
						lua_pop(L, 1);
					}

				}
				this->out << "}";
			}
		}
	} jsonloop(L);

	assert(lua_istable(L,index));

	jsonloop.loop(index);
	return jsonloop.out.str();
}


ScriptRunner* ScriptRunner::__this(lua_State* L)
{
	return (ScriptRunner*)lua_getusertag(L);
//	lua_getfield(L,LUA_REGISTRYINDEX,"_this");
//	ScriptRunner* p = (ScriptRunner*)lua_touserdata(L,-1);
//	lua_pop(L,1);
//	return p;
}

