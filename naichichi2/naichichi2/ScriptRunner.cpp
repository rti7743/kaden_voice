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
	//メモリ解放
	CDeleteAll(this->callbackHistoryList);
}

//インスタンスの再読み込み
//プログラムをデバッグしているときとか、再読み込み機能がないと死ねるから・・・
xreturn::r<bool> ScriptRunner::Reload()
{
	CForeach(this->callbackHistoryList,{
			if (this->PoolMainWindow->Recognition.RemoveCallback(_,false))
			{
				return true;
			}
			if (this->PoolMainWindow->Speak.RemoveCallback(_,false))
			{
				return true;
			}
			if (this->PoolMainWindow->Httpd.RemoveCallback(_,false))
			{
				return true;
			}
	});

	//Luaを飛ばす.
	if (this->LuaInstance)
	{
		lua_close(this->LuaInstance);
		this->LuaInstance = NULL;
	}
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
	lua_register(this->LuaInstance, "webload", (lua_CFunction)ScriptRunner::l_webload);
	lua_register(this->LuaInstance, "weblocation", (lua_CFunction)ScriptRunner::l_weblocation);
	lua_register(this->LuaInstance, "weberror", (lua_CFunction)ScriptRunner::l_weberror);
	lua_register(this->LuaInstance, "webecho", (lua_CFunction)ScriptRunner::l_webecho);
	lua_register(this->LuaInstance, "webecho_table", (lua_CFunction)ScriptRunner::l_webecho_table);
	lua_register(this->LuaInstance, "json_encode", (lua_CFunction)ScriptRunner::l_json_encode);
	lua_register(this->LuaInstance, "json_decode", (lua_CFunction)ScriptRunner::l_json_decode);
	lua_register(this->LuaInstance, "xml_encode", (lua_CFunction)ScriptRunner::l_xml_encode);
	lua_register(this->LuaInstance, "xml_decode", (lua_CFunction)ScriptRunner::l_xml_decode);
	lua_register(this->LuaInstance, "gotoweb", (lua_CFunction)ScriptRunner::l_gotoweb);
	lua_register(this->LuaInstance, "tips", (lua_CFunction)ScriptRunner::l_tips);
	lua_register(this->LuaInstance, "findmedia", (lua_CFunction)ScriptRunner::l_findmedia);
	lua_register(this->LuaInstance, "webmenu", (lua_CFunction)ScriptRunner::l_webmenu);
	lua_register(this->LuaInstance, "webmenusub", (lua_CFunction)ScriptRunner::l_webmenusub);
	lua_register(this->LuaInstance, "getwebmenu", (lua_CFunction)ScriptRunner::l_getwebmenu);
	lua_register(this->LuaInstance, "callwebmenu", (lua_CFunction)ScriptRunner::l_callwebmenu);

	//thisの保存
//	lua_pushlightuserdata(this->LuaInstance ,(void*) this);
//	lua_setfield(this->LuaInstance,LUA_REGISTRYINDEX,"_this");
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

	
	for ( auto it = this->callbackHistoryList.begin(); it != this->callbackHistoryList.end() ; ++it)
	{
		if (*it == callback)
		{
			delete *it;
			this->callbackHistoryList.erase(it);
			break;
		}
	}
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

	auto xr = _this->PoolMainWindow->ActionScriptManager.Regist(_this->CreateCallback( func ) , action);
	if(!xr)
	{
		return luaL_errorHelper(L,lua_funcdump(L,"action") + "実行時のエラー:" +xr.getErrorMessage());
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

//web専用 テンプレートを読み込む
int ScriptRunner::l_webload(lua_State* L)
{
	ScriptRunner* _this = __this(L);
//	_this->PoolMainWindow->SyncInvokeLog(std::string() + "lua function:" + lua_funcdump(L,"webload") ,LOG_LEVEL_DEBUG);

	//テンプレート名
	if (! lua_isstring(L,-2) )
	{
		return luaL_errorHelper(L,lua_funcdump(L,"webload") + "の第1引数が文字列ではありません");
	}
	//table
	if (! lua_istable(L,-1) )
	{
		return luaL_errorHelper(L,lua_funcdump(L,"webload") + "の第2引数がテーブルではありません");
	}
	//tableをすべてエスケープする。
	//動的にテーブルを構築するのはスタックが汚れてしまって無理があるので、
	//一度配列に構築した後、それをテーブルとして複写する.
	//一次元のリストだが、配列の操作を取得時と、再構築時に同じことを行うわけだから、次元は無視してもよい。
	struct _temp
	{
		std::string key;
		std::string value;
		enum type
		{
			 _temp_type_value
			,_temp_type_nest
			,_temp_type_up
		}
		type;
	};
	struct _nest_lamba
	{
		lua_State* L;
		int index;

		_nest_lamba(lua_State* L,int index) : L(L) , index(index) { func(); };
		//再起するのでクラス内クラスで。
		void func()
		{
			_USE_WINDOWS_ENCODING;
			lua_pushnil(L);
			while (lua_next(L, index - 1) != 0) 
			{
				_temp* p = new _temp;
#ifdef _WINDOWS
				p->key = _U2A(lua_tostring(L, -2));
#else
				p->key = lua_tostring(L, -2);
#endif
				if (lua_istable(L, -1))
				{
					p->type = _temp::_temp_type_nest;
					safeArray.push_back(p);

					func();

					_temp* p2 = new _temp;
					p2->type = _temp::_temp_type_up;
					safeArray.push_back(p2);
				}
				else
				{
					p->type = _temp::_temp_type_value;
					if (p->key.size() >= 2 && p->key[0] == '_' && p->key[1] == '_')
					{//キーが __hogehoge のように、 __ で始まる場合のみ自動エスケープをしない.
						p->value = lua_dump(L,-1 , false,0);
					}
					else
					{
						p->value = XLStringUtil::htmlspecialchars(lua_dump(L,-1 ,false, 0));
					}
					safeArray.push_back(p);
				}

				lua_pop(L, 1);
			}
		}
		std::list< _temp* > safeArray;
	} 
	//テーブルの読み込み処理
	nest(L,-1);

	//テンプレートの読み込み
	std::string functionanme = "webtemplate";
	std::string filename = lua_tostringHelper(L,-2);
	filename = _this->PoolMainWindow->Httpd.WebPathToRealPath(filename);
	std::string tplcode = convertTemplate(filename,functionanme);

	//ここからスタックを破壊するので、関数名を避難させる。
	std::string func = lua_funcdump(L,"webload");

	//テンプレートコードの読み込み
	if ( luaL_loadstring(L, tplcode.c_str()) )
	{
		return luaL_errorHelper(L,func + "のテンプレート" + filename + " の解析中にエラー。 Lua:" + lua_tostringHelper(L, -1));
	}
	//まずそのスクリプトを実行させて、 functionanme を登録します。
	if ( lua_pcall( L, 0, 0, 0 ) )
	{
		return luaL_errorHelper(L,func + "のテンプレート" + filename + " の実行中にエラー。 Lua:" + lua_tostringHelper(L, -1));
	}
	//functionanme を呼び出す準備に入ります。
	lua_getglobal(L, functionanme.c_str() );
	if(!lua_isfunction(L,-1) )
	{
		return luaL_errorHelper(L,func + "のテンプレート" + filename + " を実行しましたが、内部用関数" + functionanme + "が登録されていません。 Lua:" + lua_tostringHelper(L, -1));
	}
	//テンプレート引数としてのテーブルを具現化
	//thank http://logsoku.com/thread/pc2.2ch.net/tech/1063711237/824
	lua_newtable(L); 
	for(auto it = nest.safeArray.begin() ; it != nest.safeArray.end() ; ++it)
	{
		if ((*it)->type == _temp::_temp_type_value)
		{
			lua_pushstringHelper(L, (*it)->key);
			lua_pushstringHelper(L, (*it)->value);
			lua_settable(L, -3);
		}
		else if ((*it)->type == _temp::_temp_type_nest)
		{
			lua_pushstringHelper(L, (*it)->key);
			lua_newtable(L);
		}
		else //if ((*it)->type == _temp::_temp_type_up)
		{
			lua_settable(L, -3);
		}
		delete *it;
	}

	//関数呼び出し
	if ( lua_pcall( L, 1, 0, 0 ) )
	{
		std::string a = lua_tostringHelper(L, -1);
		return luaL_errorHelper(L,func + "のテンプレート" + filename + " の実行中にエラー。 Lua:" + lua_tostringHelper(L, -1));
	}

	return 0;             //戻り値の数を指定
}


//web専用 ロケーションさせる
int ScriptRunner::l_weblocation(lua_State* L)
{
	ScriptRunner* _this = __this(L);
	_this->PoolMainWindow->SyncInvokeLog(std::string() + "lua function:" + lua_funcdump(L,"weblocation") ,LOG_LEVEL_DEBUG);

	//URL
	if (! lua_isstring(L,-1) )
	{
		return luaL_errorHelper(L,lua_funcdump(L,"weblocation") + "の第1引数が文字列ではありません");
	}
	_this->WebechoStdout = lua_tostring(L,-1); //URL UTF8のままほしい
	_this->WebResultType = WEBSERVER_RESULT_TYPE_LOCATION;

	return 0;             //戻り値の数を指定
}

//web専用 エラーを発生させる.
int ScriptRunner::l_weberror(lua_State* L)
{
	ScriptRunner* _this = __this(L);
	_this->PoolMainWindow->SyncInvokeLog(std::string() + "lua function:" + lua_funcdump(L,"weberror") ,LOG_LEVEL_DEBUG);

	int argc = lua_gettop(L);
	if (argc >= 1)
	{
		//code
		auto code = ScriptRunner::lua_crossdataToString(L ,-1);
		if (!code)
		{
			return luaL_errorHelper(L,lua_funcdump(L,"weberror") + "の第1引数が文字列または数字ではありません");
		}
		_this->WebResultType = WEBSERVER_RESULT_TYPE_ERROR;
		_this->WebechoStdout = code;
	}
	else
	{
		_this->WebResultType = WEBSERVER_RESULT_TYPE_ERROR;
		_this->WebechoStdout = "500";
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

	//拡張子
	const std::string ext = XLStringUtil::strtolower(XLStringUtil::baseext_nodot( command ));
	//動作を変更する拡張子はあるかな？ めったに使わないので、これでも速度に影響はないだろう。
	const std::string overraideCommand =_this->PoolMainWindow->Config.Get(std::string("action__executeext_") + ext, "");
	if (! overraideCommand.empty() )
	{//あるので動作を上書きする.
		args = XLStringUtil::chop( XLStringUtil::escapeshellarg(command) + " " + args );
		command = overraideCommand;
	}

	auto r = ActionImplement::Execute("",command,args , baseDirectory );
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
int ScriptRunner::l_webecho(lua_State* L)
{
	ScriptRunner* _this = __this(L);

	int argc = lua_gettop(L) * -1;
	for(;argc < 0;argc++)
	{
		_this->WebechoStdout += lua_dump(L , argc,true,0);
	}

	return 0;
}

int ScriptRunner::l_webecho_table(lua_State* L)
{
	ScriptRunner* _this = __this(L);
	_this->PoolMainWindow->SyncInvokeLog(std::string() + "lua function:" + lua_funcdump(L,"webecho_table") ,LOG_LEVEL_DEBUG);

	int p1index = 0;
	const char * p2 = NULL;
	const char * p3 = NULL;
	int argc = lua_gettop(L) * -1;
	if (argc == 3)
	{
		//第1引数 出力するテーブル
		if (! lua_istable(L,-3) )
		{
			return luaL_errorHelper(L,lua_funcdump(L,"webecho_table") + "の第1引数がテーブルではありません");
		}
		//第2引数 種類
		if (! lua_isstring(L,-2) )
		{
			return luaL_errorHelper(L,lua_funcdump(L,"webecho_table") + "の第2引数が文字列ではありません");
		}
		//第3引数 戻り値関数
		if (! lua_isstring(L,-1) )
		{
			return luaL_errorHelper(L,lua_funcdump(L,"webecho_table") + "の第3引数が文字列ではありません");
		}
		p1index = -3;
		p2 = lua_tostring(L,-2);
		p3 = lua_tostring(L,-1);
	}
	else
	{
		//第1引数 出力するテーブル
		if (! lua_istable(L,-2) )
		{
			return luaL_errorHelper(L,lua_funcdump(L,"webecho_table") + "の第1引数がテーブルではありません");
		}
		//第2引数 種類
		if (! lua_isstring(L,-1) )
		{
			return luaL_errorHelper(L,lua_funcdump(L,"webecho_table") + "の第1引数が文字列ではありません");
		}
		p1index = -2;
		p2 = lua_tostring(L,-1);
		assert(p3 == NULL);

	}

	//ヘッダーをつける.
	if (strcmp(p2 , "json") == 0)
	{
		_this->WebHeaders += "Content-Type: text/javascript; charset=utf-8\r\n";
		_this->WebechoStdout += lua_tojson(L,p1index);
	}
	else if (strcmp(p2 , "jsonp") == 0)
	{
		if (p3 == NULL)
		{
			return luaL_errorHelper(L,lua_funcdump(L,"webechoex") + " jsonpが指定されたときは関数名を第3引数に渡す必要があります");
		}
		_this->WebHeaders += "Content-Type: text/javascript; charset=utf-8\r\n";
		_this->WebechoStdout += "<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" /><script type=\"text/javascript\">";
		_this->WebechoStdout += p3;
		_this->WebechoStdout += "(";
		_this->WebechoStdout += lua_tojson(L,p1index);
		_this->WebechoStdout += ");</script></head><body></body></html>";
	}
	else if (strcmp(p2 , "xml") == 0)
	{
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
	ScriptRunner* _this = __this(L);
	_this->PoolMainWindow->SyncInvokeLog(std::string() + "lua function:" + lua_funcdump(L,"findmedia") ,LOG_LEVEL_DEBUG);

	auto r1 = ScriptRunner::lua_crossdataToString(L ,-4);
	if (!r1)
	{
		return luaL_errorHelper(L,lua_funcdump(L,"findmedia") + "の第1引数が文字列ではありません");
	}
	std::string query = r1;

	if (! lua_isnumber(L,-3) )
	{
		return luaL_errorHelper(L,lua_funcdump(L,"findmedia") + "の第2引数が数字ではありません");
	}
	unsigned int limitfrom = (unsigned int) lua_tonumber(L,-3);

	if (! lua_isnumber(L,-2) )
	{
		return luaL_errorHelper(L,lua_funcdump(L,"findmedia") + "の第3引数が数字ではありません");
	}
	unsigned int limitto = (unsigned int) lua_tonumber(L,-2);

	auto r4 = ScriptRunner::lua_crossdataToString(L ,-1);
	if (!r4)
	{
		return luaL_errorHelper(L,lua_funcdump(L,"findmedia") + "の第4引数が文字列ではありません");
	}
	std::string type = r4;

	//クエリー投げて、結果をlua tableに変換します。
	lua_newtable(L);
	lua_pushstringHelper(L, "header_from" );
	lua_pushunsigned(L,limitfrom + 1 );
	lua_settable(L, -3);
	lua_pushstringHelper(L, "header_to" );
	lua_pushunsigned(L,limitto + 1 );
	lua_settable(L, -3);

	int count = limitfrom + 1;
	_this->PoolMainWindow->Media.SearchQuery(query,limitfrom,limitto,type,
			[&](const MediaFileIndex::SearchResult& sr) -> bool {
					//2次元配列
					lua_pushstringHelper(L, num2str(count) );
					lua_newtable(L);
					{
						lua_pushstring(L, "sizehash" );
						lua_pushunsigned(L,sr.sizehash );
						lua_settable(L, -3);

						lua_pushstring(L, "part" );
						lua_pushunsigned(L, sr.part );
						lua_settable(L, -3);

						lua_pushstring(L, "dir" );
						//lua_pushstringHelper(L, sr.dir );
						lua_pushstring(L, sr.dir );
						lua_settable(L, -3);

						lua_pushstring(L, "filename" );
						//lua_pushstringHelper(L, sr.filename );
						lua_pushstring(L, sr.filename );
						lua_settable(L, -3);

						lua_pushstring(L, "title" );
						//lua_pushstringHelper(L, sr.title );
						lua_pushstring(L, sr.title );
						lua_settable(L, -3);

						lua_pushstring(L, "artist" );
						//lua_pushstringHelper(L, sr.artist );
						lua_pushstring(L, sr.artist );
						lua_settable(L, -3);

						lua_pushstring(L, "album" );
						//lua_pushstringHelper(L, sr.album );
						lua_pushstring(L, sr.album );
						lua_settable(L, -3);

						lua_pushstring(L, "alias" );
						//lua_pushstringHelper(L, sr.alias );
						lua_pushstring(L, sr.alias );
						lua_settable(L, -3);

						lua_pushstring(L, "image" );
						//lua_pushstring(L, sr.image.c_str()  ); //base64
						lua_pushstring(L, sr.image  ); //base64
						lua_settable(L, -3);

						lua_pushstring(L, "summary" );
						//lua_pushstringHelper(L, "123" );
						lua_pushstring(L, sr.summary );
						lua_settable(L, -3);

						lua_pushstring(L, "mark" );
						lua_pushunsigned(L,count );		//何番目と読み上げに対応するマーク
						lua_settable(L, -3);
					}
					lua_settable(L, -3);
					count ++;
					return true;
			}
	);

	return 1;
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

	auto configmap = _this->PoolMainWindow->Config.FindGetsToMap(key);

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

SEXYTEST("ScriptRunner::convertTemplate")
{
	/*
	{
		std::string a = ScriptRunner::convertTemplate("./config/testdata/loop.tpl","__test");
		a = XLStringUtil::chop(a);
		SEXYTEST_EQ(a ,"function __test(out)   for key,value in out[\"args\"] do ; webecho( [[hello]] ); webecho( \"world\" ); webecho( [[!!]] );  end ;  end");
	}
	{
		std::string a = ScriptRunner::convertTemplate("./config/testdata/hello.tpl","__test");
		a = XLStringUtil::chop(a);
		SEXYTEST_EQ(a ,"function __test(out) webecho( [[hello]] ); webecho( \"world\" ); webecho( [[!!]] ); end"); 
	}
	{
		std::string a = ScriptRunner::convertTemplate("./config/testdata/comment.tpl","__test");
		a = XLStringUtil::chop(a);
		SEXYTEST_EQ(a ,"function __test(out)   for key,value in out[\"args\"] do ; webecho( [[hello]] ); webecho( \"world\" ); webecho( [[!!]] );  end ;  end"); 
	}
	*/
}



xreturn::r<std::string> ScriptRunner::callbackWebFunction(int callbackIndex,const std::map<std::string , std::string> & match,WEBSERVER_RESULT_TYPE* type,std::string* headers)
{
	this->WebechoStdout = "";
	this->WebHeaders = "";
	this->WebResultType = WEBSERVER_RESULT_TYPE_OK;

	auto r = callbackFunction(callbackIndex,match);
	if (!r)
	{
		return xreturn::error(r.getError());
	}
	
	*type = this->WebResultType;
	*headers = this->WebHeaders;
	return this->WebechoStdout;
}


xreturn::r<std::string> ScriptRunner::callbackFunction(int callbackIndex,const std::map<std::string , std::string> & match)
{
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
		return "";
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


int ScriptRunner::l_webmenu(lua_State* L)
{
	ScriptRunner* _this = __this(L);
	_this->PoolMainWindow->SyncInvokeLog(std::string() + "lua function:" + lua_funcdump(L,"webmenu") ,LOG_LEVEL_DEBUG);

	if (!_this->IsScenario)
	{
		return luaL_errorHelper(L,lua_funcdump(L,"webmenu") + "はシナリオモードのみ有効です");
	}

	//第1引数
	if (! lua_isstring(L,-2) )
	{
		return luaL_errorHelper(L,lua_funcdump(L,"webmenu") + "の第1引数が文字列ではありません");
	}
	std::string menuname = lua_tostringHelper(L,-2);

	//第2引数
	if (! lua_isstring(L,-1) )
	{
		return luaL_errorHelper(L,lua_funcdump(L,"webmenu") + "の第2引数が文字列ではありません");
	}
	std::string image = lua_tostringHelper(L,-1);

	_this->PoolMainWindow->WebMenu.AddMenu( menuname ,image );

	return 0;             //戻り値の数を指定
}

int ScriptRunner::l_webmenusub(lua_State* L)
{
	ScriptRunner* _this = __this(L);
	_this->PoolMainWindow->SyncInvokeLog(std::string() + "lua function:" + lua_funcdump(L,"webmenusub") ,LOG_LEVEL_DEBUG);

	if (!_this->IsScenario)
	{
		return luaL_errorHelper(L,lua_funcdump(L,"webmenusub") + "はシナリオモードのみ有効です");
	}

	//第1引数
	if (! lua_isstring(L,-4) )
	{
		return luaL_errorHelper(L,lua_funcdump(L,"webmenusub") + "の第1引数が文字列ではありません");
	}
	std::string menuname = lua_tostringHelper(L,-4);

	//第2引数
	if (! lua_isstring(L,-3) )
	{
		return luaL_errorHelper(L,lua_funcdump(L,"webmenusub") + "の第2引数が文字列ではありません");
	}
	std::string actionname = lua_tostringHelper(L,-3);

	//第3引数
	if (! lua_isstring(L,-2) )
	{
		return luaL_errorHelper(L,lua_funcdump(L,"webmenusub") + "の第3引数が文字列ではありません");
	}
	std::string image = lua_tostringHelper(L,-2);

	//第4引数
	if (! lua_isfunction(L,-1) )
	{
		return luaL_errorHelper(L,lua_funcdump(L,"webmenusub") + "の第4引数が関数ではありません");
	}
	int func = luaL_ref(L,LUA_REGISTRYINDEX);

	_this->PoolMainWindow->WebMenu.AddMenuSub( menuname ,actionname , image ,_this->CreateCallback( func ) );
	return 0;             //戻り値の数を指定
}

int ScriptRunner::l_getwebmenu(lua_State* L)
{
	ScriptRunner* _this = __this(L);
	_this->PoolMainWindow->SyncInvokeLog(std::string() + "lua function:" + lua_funcdump(L,"getwebmenu") ,LOG_LEVEL_DEBUG);

	lua_newtable(L);
	auto rooms = _this->PoolMainWindow->WebMenu.getRooms();
	for(auto roomIT = rooms->begin() ; roomIT != rooms->end() ; ++ roomIT )
	{
		lua_pushstringHelper(L, (*roomIT)->name );
		lua_newtable(L);
		{
			lua_pushstringHelper(L, "name" );
			lua_pushstringHelper(L, (*roomIT)->name );
			lua_settable(L, -3);

			lua_pushstringHelper(L, "ip" );
			lua_pushstringHelper(L, (*roomIT)->ip );
			lua_settable(L, -3);

			lua_pushstringHelper(L, "menus" );
			lua_newtable(L);
			for(auto menuIT = (*roomIT)->menus.begin() ; menuIT != (*roomIT)->menus.end() ; ++ menuIT )
			{
				lua_pushstringHelper(L, (*menuIT)->name );
				lua_newtable(L);
				{
					lua_pushstringHelper(L, "name" );
					lua_pushstringHelper(L, (*menuIT)->name );
					lua_settable(L, -3);

					lua_pushstringHelper(L, "image" );
					lua_pushstringHelper(L, (*menuIT)->image );
					lua_settable(L, -3);

					lua_pushstringHelper(L, "status" );
					lua_pushstringHelper(L, (*menuIT)->status );
					lua_settable(L, -3);

					lua_pushstringHelper(L, "actions" );
					lua_newtable(L);
					for(auto actionIT = (*menuIT)->actions.begin() ; actionIT != (*menuIT)->actions.end() ; ++ actionIT )
					{
						lua_pushstringHelper(L, (*actionIT)->name );
						lua_newtable(L);
						{
							lua_pushstringHelper(L, "name" );
							lua_pushstringHelper(L, (*actionIT)->name );
							lua_settable(L, -3);

							lua_pushstringHelper(L, "image" );
							lua_pushstringHelper(L, (*actionIT)->image );
							lua_settable(L, -3);
						}
						lua_settable(L, -3);
					}
					lua_settable(L, -3);
				}
				lua_settable(L, -3);
			}
			lua_settable(L, -3);
		}
		lua_settable(L, -3);
	}
	return 1;             //戻り値の数を指定
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

	_this->PoolMainWindow->WebMenu.Fire(roomname,menuname,actionanme);
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
CallbackDataStruct* ScriptRunner::CreateCallback(int _func)
{
	CallbackDataStruct* callback = new CallbackDataStruct(this,_func );
	this->callbackHistoryList.push_back(callback);

	return callback;
}


ScriptRunner* ScriptRunner::__this(lua_State* L)
{
	return (ScriptRunner*)lua_getusertag(L);
//	lua_getfield(L,LUA_REGISTRYINDEX,"_this");
//	ScriptRunner* p = (ScriptRunner*)lua_touserdata(L,-1);
//	lua_pop(L,1);
//	return p;
}

