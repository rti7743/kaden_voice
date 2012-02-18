#include "common.h"
#include "ScriptRunner.h"
#include "MainWindow.h"
#include "ActionImplement.h"
#include <fstream>
#include "XLStringUtil.h"

xreturn::r<bool> ScriptRunner::LoadScript(const std::string & filename)
{
	assert(this->LuaInstance == NULL);

	// Luaを開く
	this->filename = filename;
	this->LuaInstance = luaL_newstate();

	// Lua標準関数を使う
//	luaL_openlibs(this->LuaInstance);
	//ないちちで利用する関数オーバーライド
	lua_register(this->LuaInstance, "onvoice", (lua_CFunction)ScriptRunner::l_onvoice);
	lua_register(this->LuaInstance, "onvoice_local", (lua_CFunction)ScriptRunner::l_onvoice_local);
	lua_register(this->LuaInstance, "onhttp", (lua_CFunction)ScriptRunner::l_onhttp);
	lua_register(this->LuaInstance, "speak", (lua_CFunction)ScriptRunner::l_speak);
	lua_register(this->LuaInstance, "action", (lua_CFunction)ScriptRunner::l_action);
	lua_register(this->LuaInstance, "getconfig", (lua_CFunction)ScriptRunner::l_get_config);
	lua_register(this->LuaInstance, "setconfig", (lua_CFunction)ScriptRunner::l_set_config);
	lua_register(this->LuaInstance, "telnet", (lua_CFunction)ScriptRunner::l_telnet);
	lua_register(this->LuaInstance, "execute", (lua_CFunction)ScriptRunner::l_execute);
	lua_register(this->LuaInstance, "httpget", (lua_CFunction)ScriptRunner::l_httpget);
	lua_register(this->LuaInstance, "httppost", (lua_CFunction)ScriptRunner::l_httppost);
	lua_register(this->LuaInstance, "sendkey", (lua_CFunction)ScriptRunner::l_sendkey);
	lua_register(this->LuaInstance, "sendmessage", (lua_CFunction)ScriptRunner::l_sendmessage);
	lua_register(this->LuaInstance, "msleep", (lua_CFunction)ScriptRunner::l_msleep);
	lua_register(this->LuaInstance, "var_dump", (lua_CFunction)ScriptRunner::l_dump);
	lua_register(this->LuaInstance, "dump", (lua_CFunction)ScriptRunner::l_dump);
//	lua_register(this->LuaInstance, "print", (lua_CFunction)ScriptRunner::l_dump);
//	lua_register(this->LuaInstance, "echo", (lua_CFunction)ScriptRunner::l_dump);
	lua_register(this->LuaInstance, "callstack", (lua_CFunction)ScriptRunner::l_callstack);
	lua_register(this->LuaInstance, "webload", (lua_CFunction)ScriptRunner::l_webload);
	lua_register(this->LuaInstance, "weblocation", (lua_CFunction)ScriptRunner::l_weblocation);
	lua_register(this->LuaInstance, "weberror", (lua_CFunction)ScriptRunner::l_weberror);
	lua_register(this->LuaInstance, "webecho", (lua_CFunction)ScriptRunner::l_webecho);
	lua_register(this->LuaInstance, "gotoweb", (lua_CFunction)ScriptRunner::l_gotoweb);
	lua_register(this->LuaInstance, "tips", (lua_CFunction)ScriptRunner::l_tips);

	// Luaファイルを読み込む
	if(luaL_loadfile(this->LuaInstance, filename.c_str() ) )
	{
		return xreturn::error(std::string("スクリプト読み込みに失敗しました。 Filename:") + this->filename + " Lua:" + lua_tostringHelper(this->LuaInstance, -1));
	}

	//thisの保存
	lua_pushlightuserdata(this->LuaInstance ,(void*) this);
	lua_setfield(this->LuaInstance,LUA_REGISTRYINDEX,"_this");

	//スクリプトのグローバルなエリアの処理.
	if(lua_pcall(this->LuaInstance, 0, 0, 0) )
	{
		return xreturn::error(std::string("スクリプト読み込み時の実行に失敗しました。 Filename:") + this->filename + " Lua:" + lua_tostringHelper(this->LuaInstance, -1));
	}

	return true;
}


//テーブルからデータを読みます。
xreturn::r<std::map<std::string,std::string> > ScriptRunner::readLuaTable(const std::string & name,int or_id )
{
//		ASSERT(this->LuaInstance != NULL);
	std::map<std::string,std::string> ret;
	
	//see http://blogs.wankuma.com/izmktr/archive/2009/03/27/170306.aspx
	if(name.empty())
	{
		lua_settable(this->LuaInstance, or_id);
	}
	else
	{
		lua_getglobal(this->LuaInstance, name.c_str() );
	}

	if(!lua_istable(this->LuaInstance, -1)) 
	{
		return xreturn::error((std::string("変数") + name + "はテーブルではありません。" + " Filename:") + this->filename + " Lua:" + lua_tostringHelper(this->LuaInstance, -1));
	}

	lua_pushnil(this->LuaInstance);
	while (lua_next(this->LuaInstance, -2) != 0) 
	{
		lua_pushvalue(this->LuaInstance, -2);

		const std::string key = lua_tostringHelper(this->LuaInstance, -1);
		//値
		auto value = ScriptRunner::lua_crossdataToString(this->LuaInstance ,-2);
		if (value.isOK())
		{
			ret[key] = value;
		}

		lua_pop(this->LuaInstance, 2);
	}
	lua_pop(this->LuaInstance, 1);

	return ret;
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

//コールバックが不要になった時
void ScriptRunner::unrefCallback(int func)
{
	assert (func != NO_CALLBACK);

	luaL_unref(this->LuaInstance,LUA_REGISTRYINDEX, func);
}

int ScriptRunner::l_onvoice(lua_State* L)
{
	ScriptRunner* _this = __this(L);
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

	auto xr = _this->PoolMainWindow->Recognition.AddCommandRegexp( CallbackDataStruct(_this ,func ) , p1);
	if(!xr)
	{
		return luaL_errorHelper(L,lua_funcdump(L,"onvoice") + "実行時のエラー:" +xr.getErrorMessage());
	}

	return 0;             //戻り値の数を指定
}

int ScriptRunner::l_onvoice_local(lua_State* L)
{
	ScriptRunner* _this = __this(L);
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

 
	auto xr = _this->PoolMainWindow->Recognition.AddTemporaryRegexp( CallbackDataStruct(_this ,func ) , p1);
	if(!xr)
	{
		return luaL_errorHelper(L,lua_funcdump(L,"onvoice_local") + "実行時のエラー" +xr.getErrorMessage());
	}

	return 0;             //戻り値の数を指定
}

int ScriptRunner::l_onhttp(lua_State* L)
{
	ScriptRunner* _this = __this(L);
	if (!_this->IsScenario)
	{
		return luaL_errorHelper(L,lua_funcdump(L,"onhttp") + "はシナリオモードのみ有効です");
	}
	int argc = lua_gettop(L);
	std::string key;
	int func;

	if (argc == 1)
	{
		//第1引数
		if (! lua_isstring(L,-1) )
		{
			return luaL_errorHelper(L,lua_funcdump(L,"onhttp") + "の第1引数が文字列ではありません");
		}
		key = lua_tostringHelper(L,-1);

		func = NO_CALLBACK;
	}
	else if (argc == 2)
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

	
	auto xr = _this->PoolMainWindow->Httpd.Regist(CallbackDataStruct(_this ,func ) , key);
	if(!xr)
	{
		return luaL_errorHelper(L,lua_funcdump(L,"onhttp") + "実行時のエラー" +xr.getErrorMessage());
	}

	return 0;             //戻り値の数を指定
}

int ScriptRunner::l_action(lua_State* L)
{
	ScriptRunner* _this = __this(L);
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

	auto xr = _this->PoolMainWindow->ActionScriptManager.Regist(CallbackDataStruct(_this ,func ) , action);
	if(!xr)
	{
		return luaL_errorHelper(L,lua_funcdump(L,"action") + "実行時のエラー:" +xr.getErrorMessage());
	}

	return 0;             //戻り値の数を指定
}

int ScriptRunner::l_speak(lua_State* L)
{
	ScriptRunner* _this = __this(L);

	std::string text;

	int argc = lua_gettop(L);
	if (argc == 1)
	{
		//第1引数
		if (! lua_isstring(L,-1) )
		{
			return luaL_errorHelper(L,lua_funcdump(L,"speak") + "の第1引数が文字列ではありません");
		}
		text = lua_tostringHelper(L,-1);
	}
	else
	{
		return luaL_errorHelper(L,lua_funcdump(L,"speak") + "の引数数が正しくありません。");
	}

	auto xr = _this->PoolMainWindow->Speak.Speak(text);
	if(!xr)
	{
		return luaL_errorHelper(L,lua_funcdump(L,"speak") + "実行時のエラー:" +xr.getErrorMessage());
	}

	return 0;             //戻り値の数を指定
}


int ScriptRunner::l_ontimer(lua_State* L)
{
	ScriptRunner* _this = __this(L);
	if (!_this->IsScenario)
	{
		return luaL_errorHelper(L,lua_funcdump(L,"ontimer") + "はシナリオモードのみ有効です");
	}

	//第1引数
	int p1 = luaL_checkint(L,-2);
	if (!p1)
	{
		return luaL_errorHelper(L,lua_funcdump(L,"ontimer") + "の第1引数が文字列ではありません");
	}

	//第2引数
	if (! lua_isfunction(L,-1) )
	{
		return luaL_errorHelper(L,lua_funcdump(L,"ontimer") + "の第2引数が関数ではありません");
	}
	int func = luaL_ref(L,LUA_REGISTRYINDEX);


//	_this(L)->KeepTimerManager->AddTimer( func , p1 );
	return 0;             //戻り値の数を指定
}

int ScriptRunner::l_ontrigger(lua_State* L)
{
	ScriptRunner* _this = __this(L);
	if (!_this->IsScenario)
	{
		return luaL_errorHelper(L,lua_funcdump(L,"ontrigger") + "はシナリオモードのみ有効です。");
	}

	//第1引数
	int p1 = luaL_checkint(L,-2);
	if (!p1)
	{
		return luaL_errorHelper(L,lua_funcdump(L,"ontrigger") + "の第1引数が文字列ではありません");
	}

	//第2引数
	if (! lua_isfunction(L,-1) )
	{
		return luaL_errorHelper(L,lua_funcdump(L,"ontrigger") + "の第2引数が関数ではありません");
	}
	int func = luaL_ref(L,LUA_REGISTRYINDEX);


//	_this(L)->PoolMainWindow->->AddTrigger( func , p1 );
	return 0;             //戻り値の数を指定
}

int ScriptRunner::l_onidle(lua_State* L)
{
	ScriptRunner* _this = __this(L);
	if (!_this->IsScenario)
	{
		return luaL_errorHelper(L,lua_funcdump(L,"onidle") + "はシナリオモードのみ有効です。");
	}

	//第1引数
	int p1 = luaL_checkint(L,-2);
	if (!p1)
	{
		return luaL_errorHelper(L,lua_funcdump(L,"onidle") + "の第1引数が文字列ではありません");
	}

	//第2引数
	if (! lua_isfunction(L,-1) )
	{
		return luaL_errorHelper(L,lua_funcdump(L,"onidle") + "の第2引数が関数ではありません");
	}
	int func = luaL_ref(L,LUA_REGISTRYINDEX);

//	_this(L)->KeepScriptManager->AddIdle( func , p1 );
	return 0;             //戻り値の数を指定
}

//web専用 テンプレートを読み込む
int ScriptRunner::l_webload(lua_State* L)
{
	ScriptRunner* _this = __this(L);

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
					func();
				}
				else
				{
					p->type = _temp::_temp_type_nest;
					if (p->key.size() >= 2 && p->key[0] == '_' && p->key[1] == '_')
					{//キーが __hogehoge のように、 __ で始まる場合のみ自動エスケープをしない.
						p->value = lua_dump(L,-1 , 0);
					}
					else
					{
						p->value = XLStringUtil::htmlspecialchars(lua_dump(L,-1 , 0));
					}
				}
				safeArray.push_back(p);

				lua_pop(L, 1);
			}
			_temp* p = new _temp;
			p->type = _temp::_temp_type_up;
			safeArray.push_back(p);
		}
		std::list< _temp* > safeArray;
	} 
	//テーブルの読み込み処理
	nest(L,-1);

	//テンプレートの読み込み
	std::string filename = lua_tostringHelper(L,-2);
	filename = _this->PoolMainWindow->Httpd.WebPathToRealPath(filename);
	std::string tplcode = convertTemplate(filename);

	//ここからスタックを破壊するので、関数名を避難させる。
	std::string func = lua_funcdump(L,"webload");

	//テンプレートコードの読み込み
	if ( luaL_loadstring(L, tplcode.c_str()) )
	{
		return luaL_errorHelper(L,func + "のテンプレート" + filename + " の解析中にエラー。 Lua:" + lua_tostringHelper(L, -1));
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
		return luaL_errorHelper(L,func + "のテンプレート" + filename + " の実行中にエラー。 Lua:" + lua_tostringHelper(L, -1));
	}

	return 0;             //戻り値の数を指定
}

//web専用 ロケーションさせる
int ScriptRunner::l_weblocation(lua_State* L)
{
	ScriptRunner* _this = __this(L);

	//URL
	if (! lua_isstring(L,-1) )
	{
		return luaL_errorHelper(L,lua_funcdump(L,"weblocation") + "の第1引数が文字列ではありません");
	}
	_this->WebechoStdout = lua_tostringHelper(L,-1); //URL
	_this->WebResultType = WEBSERVER_RESULT_TYPE_LOCATION;

	return 0;             //戻り値の数を指定
}

//web専用 エラーを発生させる.
int ScriptRunner::l_weberror(lua_State* L)
{
	ScriptRunner* _this = __this(L);
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
		_this->PoolMainWindow->SyncInvokeLog( lua_dump(L , argc,0) ,LOG_LEVEL_NOTIFY);
	}

	return 0;
}
int ScriptRunner::l_webecho(lua_State* L)
{
	ScriptRunner* _this = __this(L);

	int argc = lua_gettop(L) * -1;
	for(;argc < 0;argc++)
	{
		_this->WebechoStdout += lua_dump(L , argc,0);
	}
	_this->WebResultType = WEBSERVER_RESULT_TYPE_OK;

	return 0;
}

//続きはwebで
int ScriptRunner::l_gotoweb(lua_State* L)
{
	ScriptRunner* _this = __this(L);

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

	std::string title;
	std::string message;

	int argc = lua_gettop(L);
	if (argc <= 0)
	{
		return luaL_errorHelper(L,lua_funcdump(L,"gotoweb") + "の第1引数が数字ではありません");
	}
	else if (argc == 1)
	{
		title = "";
		auto r1 = ScriptRunner::lua_crossdataToString(L ,-1);
		if (!r1)
		{
			return luaL_errorHelper(L,lua_funcdump(L,"gotoweb") + "の第1引数が数字ではありません");
		}
		message = r1;
	}
	else 
	{
		auto r1 = ScriptRunner::lua_crossdataToString(L ,-2);
		if (!r1)
		{
			return luaL_errorHelper(L,lua_funcdump(L,"gotoweb") + "の第1引数が数字ではありません");
		}
		title = r1;

		auto r2 = ScriptRunner::lua_crossdataToString(L ,-1);
		if (!r1)
		{
			return luaL_errorHelper(L,lua_funcdump(L,"gotoweb") + "の第2引数が数字ではありません");
		}
		message = r2;
	}

	_this->PoolMainWindow->SyncInvokePopupMessage(title,message);

	return 0;
}

int ScriptRunner::l_callstack(lua_State* L)
{
	ScriptRunner* _this = __this(L);

	_this->PoolMainWindow->SyncInvokeLog( lua_callstack(L) );

	return 0;
}

std::string ScriptRunner::convertTemplate(const std::string & filename)
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
//	out << "function(out){";
	out << "webecho ( [[";

	bool easy_echo = false;
	std::ifstream TPL( filename );
	std::string line;
	while( std::getline(TPL,line) )
	{
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
					}
					else if (*(p+2) == 'l' && *(p+3) == 'u' && *(p+4) == 'a')
					{
						//<?lua
						state = TPL_STATE_CODE;
						out << std::string(start , 0 , (int)(p - start) )  << "]] ); ";
						start = p + 4 + 1;
						p+=4;
						easy_echo = false;
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
					if ( easy_echo  )
					{
						out << std::string(start , 0 , (int)(p - start) )  << "); webecho( [[";
					}
					else 
					{
						out << std::string(start , 0 , (int)(p - start) )  << "; webecho( [[";
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
		out << std::string(start , 0 , (int)(p - start) );
	}
	if(state == TPL_STATE_HTML)
	{
		out << "]] );";
	}
//	out << "}";
	return out.str();
}

SEXYTEST("ScriptRunner::convertTemplate")
{
	{
		std::string a = ScriptRunner::convertTemplate("./config/testdata/hello.tpl");
		SEXYTEST_EQ(a ,"webecho ( [[hello]] ); webecho( \"world\" ); webecho( [[!!]] );"); 
//		"webecho ( [[hello]] ); webecho( "world" ); webecho( [[!]] );"
	}
}


//indexで渡したluaスタックにあるluaテーブルを htmlspecialcharsした安全な配列にしてスタックに積み直す.
void ScriptRunner::push_lua_SafeLuaTable(lua_State* L , int index)
{
	assert(lua_istable(L,index));

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

		_nest_lamba(lua_State* L,int index) : L(L) , index(index) {};
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
					func();
				}
				else
				{
					p->type = _temp::_temp_type_nest;
					if (p->key.size() >= 2 && p->key[0] == '_' && p->key[1] == '_')
					{//キーが __hogehoge のように、 __ で始まる場合のみ自動エスケープをしない.
						p->value = lua_dump(L,-1 , 0);
					}
					else
					{
						p->value = XLStringUtil::htmlspecialchars(lua_dump(L,-1 , 0));
					}
				}
				safeArray.push_back(p);

				lua_pop(L, 1);
			}
			_temp* p = new _temp;
			p->type = _temp::_temp_type_up;
			safeArray.push_back(p);
		}
		std::list< _temp* > safeArray;
	} 
	nest(L,index);
	nest.func();

	//arrayの取得が終わったので次は再構築を行う.
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

}

xreturn::r<std::string> ScriptRunner::callbackWebFunction(int callbackIndex,const std::map<std::string , std::string> & match,WEBSERVER_RESULT_TYPE* type)
{
	this->WebechoStdout = "";
	this->WebResultType = WEBSERVER_RESULT_TYPE_OK;

	auto r = callbackFunction(callbackIndex,match);
	if (!r)
	{
		return xreturn::error(r.getError());
	}

	return this->WebechoStdout;
}


xreturn::r<std::string> ScriptRunner::callbackFunction(int callbackIndex,const std::map<std::string , std::string> & match)
{
	//LUA_REGISTRYINDEX に預けておいたコールバックポイントの復元
	lua_rawgeti(this->LuaInstance,LUA_REGISTRYINDEX, callbackIndex);

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

std::string ScriptRunner::lua_dump(lua_State* L , int index, int nest)
{
	_USE_WINDOWS_ENCODING;

	if(lua_isstring(L,index)) 
	{//文字列
#ifdef _WINDOWS
		return _U2A(lua_tostring(L, index));
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
		std::stringstream out;
		out << "{" << std::endl;
		std::string nest_space(nest * 4, ' ');
		lua_pushnil(L);
		while (lua_next(L, index - 1) != 0) 
		{
#ifdef _WINDOWS
			std::string key = _U2A(lua_tostring(L, -2));
#else
			std::string key = lua_tostring(L, -2);
#endif
			out << nest_space << key << ":" <<  lua_dump(L,-1 , nest + 1) << std::endl;

			lua_pop(L, 1);
		}
		out << nest_space << "}";
		return out.str();
	}
#ifdef _WINDOWS
	return _A2U(lua_typename(L, lua_type(L, index)));
#else
	return lua_typename(L, lua_type(L, index));
#endif
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
	int argc = lua_gettop(L);
	for(int i = 0 ; i < argc ; ++i)
	{
		r += lua_dump(L,i * -1 , 0);
		if (i < argc) r += ",";
	}
	return r + ")";
}

ScriptRunner* ScriptRunner::__this(lua_State* L)
{
	lua_getfield(L,LUA_REGISTRYINDEX,"_this");
	ScriptRunner* p = (ScriptRunner*)lua_touserdata(L,-1);
	lua_pop(L,1);
	return p;
}

