#include "common.h"
#include "MainWindow.h"
#include "ScriptWebRunner.h"
#include "XLFileUtil.h"
#include "XLStringUtil.h"
#include "ActionImplement.h"

ScriptWebRunner::ScriptWebRunner()
{
}

ScriptWebRunner::~ScriptWebRunner()
{
}

void ScriptWebRunner::Create(MainWindow * poolMainWindow)
{
	this->PoolMainWindow = poolMainWindow;
}

xreturn::r<bool> ScriptWebRunner::Load()
{
	//設定を読み込んで
	const auto configmap = this->PoolMainWindow->Config.FindGetsToMap("elec_",false);
	for(auto it = configmap.begin() ; it != configmap.end() ; ++it )
	{
		//音声認識項目
		if (strstr(it->first.c_str() , "actionvoice") && !strstr(it->first.c_str() , "actionvoice_command") )
		{
			int key1 = -1;
			int key2 = -1;
			sscanf(it->first.c_str() , "elec_%d_action_%d_actionvoice",&key1,&key2);
			if (key1 < 0 || key2 < 0 )
			{
				continue;
			}
			if ( atoi(it->second.c_str()) < 0 )
			{
				continue;
			}
			const auto commandIT = configmap.find( std::string("elec_") + num2str(key1) + "_action_" + num2str(key2) + "_actionvoice_command" );
			if (commandIT == configmap.end())
			{
				continue;
			}


			this->PoolMainWindow->SyncInvoke([=](){
				auto xr = this->PoolMainWindow->Recognition.AddCommandRegexp( this->CreateCallback( key1,key2 ) , commandIT->second);
				if(!xr)
				{
				}
			});
		}
	}
	return true;
}


xreturn::r<std::string> ScriptWebRunner::callbackFunction(const CallbackDataStruct* callback,const std::map<std::string , std::string> & match)
{
	fireAction( callback->getFunc() , callback->getFunc2() );
	return std::string("");
}

void ScriptWebRunner::fireAction(int key1,int key2)  const
{
	const auto configmap = this->PoolMainWindow->Config.FindGetsToMap("elec_",false);
	const auto commandIT = configmap.find( std::string("elec_") + num2str(key1) + "_action_" + num2str(key2) + "_actionexecuteflag" );
	if (commandIT == configmap.end())
	{
		return ;
	}

	{
		const auto tospeakIT = configmap.find( std::string("elec_") + num2str(key1) + "_action_" + num2str(key2) + "_tospeak" );
		if (tospeakIT == configmap.end())
		{
			return ;
		}
		if (atoi(tospeakIT->second.c_str()) >= 1)
		{
			const auto tospeak_selectIT = configmap.find( std::string("elec_") + num2str(key1) + "_action_" + num2str(key2) + "_tospeak_select" );
			if (tospeak_selectIT == configmap.end())
			{
				return ;
			}
			if (tospeak_selectIT->second == "文字列")
			{
				const auto tospeak_stringIT = configmap.find( std::string("elec_") + num2str(key1) + "_action_" + num2str(key2) + "_tospeak_string" );
				if (tospeak_selectIT == configmap.end())
				{
					return ;
				}
				//読み上げ
				//tospeak_selectIT->second
			}
			else if (tospeak_selectIT->second == "音楽ファイル")
			{
				const auto tospeak_mp3IT = configmap.find( std::string("elec_") + num2str(key1) + "_action_" + num2str(key2) + "_tospeak_mp3" );
				if (tospeak_mp3IT == configmap.end())
				{
					return ;
				}
				//mp3
				//tospeak_mp3IT->second
			}
		}
	}



	if ( commandIT->second == "赤外線" )
	{

	}
	else if ( commandIT->second == "赤外線_プリセット" )
	{

	}
	else if ( commandIT->second == "コマンド" )
	{
		const auto execcommandIT = configmap.find( std::string("elec_") + num2str(key1) + "_action_" + num2str(key2) + "_actionexecuteflag_command" );
		const auto execargs1IT = configmap.find( std::string("elec_") + num2str(key1) + "_action_" + num2str(key2)   + "_actionexecuteflag_command_args1" );
		const auto execargs2IT = configmap.find( std::string("elec_") + num2str(key1) + "_action_" + num2str(key2)   + "_actionexecuteflag_command_args2" );
		const auto execargs3IT = configmap.find( std::string("elec_") + num2str(key1) + "_action_" + num2str(key2)   + "_actionexecuteflag_command_args3" );
		const auto execargs4IT = configmap.find( std::string("elec_") + num2str(key1) + "_action_" + num2str(key2)   + "_actionexecuteflag_command_args4" );
		const auto execargs5IT = configmap.find( std::string("elec_") + num2str(key1) + "_action_" + num2str(key2)   + "_actionexecuteflag_command_args5" );

		const std::string execcommand = execcommandIT == configmap.end() ? "" : execcommandIT->second;
		const std::string execargs1   = execcommandIT == configmap.end() ? "" : execargs1IT->second;
		const std::string execargs2   = execcommandIT == configmap.end() ? "" : execargs2IT->second;
		const std::string execargs3   = execcommandIT == configmap.end() ? "" : execargs3IT->second;
		const std::string execargs4   = execcommandIT == configmap.end() ? "" : execargs4IT->second;
		const std::string execargs5   = execcommandIT == configmap.end() ? "" : execargs5IT->second;

		//ベースディレクトリ
		std::string baseDirectory = this->PoolMainWindow->Config.GetBaseDirectory();
		if ( XLStringUtil::strpos(execcommand,"http://") == 0 || XLStringUtil::strpos(execcommand,"https://") == 0 )
		{
			ActionImplement::OpenWeb("",execcommand);
			return ;
		}

		const std::string fullpath = XLStringUtil::pathcombine(baseDirectory,execcommand);

		//拡張子
		const std::string ext = XLStringUtil::strtolower(XLStringUtil::baseext_nodot( execcommand ));
		if (ext == "lua")
		{
			ScriptRunner runner(this->PoolMainWindow , false );
			runner.LoadScript(fullpath);

			const std::list<std::string> args;
			auto r = runner.callFunction("call",args,true);
			if (!r)
			{
//				return luaL_errorHelper(L,lua_funcdump(L,"execute") + "がエラーになりました。 " + r.getFullErrorMessage() );
				return ;
			}
		}
		else
		{
			const std::string args = "";
			auto r = ActionImplement::Execute("",fullpath,args);
			if (!r)
			{
//				return luaL_errorHelper(L,lua_funcdump(L,"execute") + "がエラーになりました。 " + r.getFullErrorMessage() );
				return ;
			}
		}
	}
}


//リモコンのweb index
std::string ScriptWebRunner::RemoconIndex(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) const
{
	const std::string filename = this->PoolMainWindow->Httpd.WebPathToRealPath("remocon.tpl");
	const std::string html = XLFileUtil::cat(filename);

	return XLStringUtil::replace_low(html,"%REMOCON%", RemoconStatus() );
}

//実行!!
std::string ScriptWebRunner::remocon_fire_byid(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) const
{
	const std::map<std::string,std::string> request = httpHeaders.getRequest();
	{
		//key1=hogehoge&key2=hogehoge
		auto actioneditable_key1IT = request.find("key1");
		if (actioneditable_key1IT == request.end())
		{
			return RemoconStatusError(20001 , "key1が見つかりません");
		}
		int key1 = atoi(actioneditable_key1IT->second.c_str());
		if (key1 <= 0) 
		{
			return RemoconStatusError(20002 , "key1の値が数字ではありません");
		}

		auto actioneditable_key2IT = request.find("key2");
		if (actioneditable_key2IT == request.end())
		{
			return RemoconStatusError(20003 , "key2が見つかりません");
		}
		int key2 = atoi(actioneditable_key2IT->second.c_str());
		if (key2 <= 0) 
		{
			return RemoconStatusError(20004 , "key2の値が数字ではありません");
		}

		fireAction(key1,key2);
		return "";
	}
}

//実行!!
std::string ScriptWebRunner::remocon_fire_byname(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) const
{
	const std::map<std::string,std::string> request = httpHeaders.getRequest();
	{
		//name1=hogehoge&name2=hogehoge
		auto name1IT = request.find("name1");
		if (name1IT == request.end())
		{
			return RemoconStatusError(20001 , "name1が見つかりません");
		}

		auto name2IT = request.find("name2");
		if (name2IT == request.end())
		{
			return RemoconStatusError(20003 , "name2が見つかりません");
		}

		int key1 = -1;
		const auto configmap = this->PoolMainWindow->Config.FindGetsToMap("elec_",false);
		for(auto it = configmap.begin() ; it != configmap.end() ; ++it )
		{
			if ( it->second == name1IT->second )
			{
				if ( ! strstr( it->first.c_str() , "_name" ) && !strstr( it->first.c_str() , "_action_" ) )
				{
					continue;
				}
				sscanf(it->first.c_str(),"elec_%d_name",&key1);
				break;
			}
		}
		if (key1 <= 0)
		{
			return "";
		}

		int key2 = -1;
		const std::string prefix = "elec_" + num2str(key1) + "_action_";
		for(auto it = configmap.begin() ; it != configmap.end() ; ++it )
		{
			if ( it->second == name2IT->second )
			{
				if ( ! strstr( it->first.c_str() , "_name" ) && strstr( it->first.c_str() , prefix.c_str() ) )
				{
					continue;
				}
				sscanf(it->first.c_str(),"elec_%d_action_%d_name",&key1,&key2);
				break;
			}
		}
		if (key1 <= 0 || key2 <= 0 )
		{
			return "";
		}

		fireAction(key1,key2);
		return "";
	}
}

//アイコンの並び順の更新
std::string ScriptWebRunner::remocon_update_icon_order(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) const
{
	const std::map<std::string,std::string> request = httpHeaders.getRequest();
	{
		boost::unique_lock<boost::mutex> al(this->updatelock);

		//elec_ID_order=hogehoge&elec_ID_order=hogehoge&elec_ID_order=hogehoge&elec_ID_order=hogehoge&elec_ID_order=hogehoge&elec_ID_order=hogehoge&
		for(auto it = request.begin() ; it != request.end() ; ++it )
		{
			int key = -1;
			sscanf(it->first.c_str(),"elec_%d_order" , &key );
			if (key == -1)
			{
				continue;
			}
			if ( atoi( it->second.c_str() ) <= 0 )
			{
				continue;
			}

			this->PoolMainWindow->Config.Set( it->first.c_str() , it->second.c_str() );
		}

		return this->RemoconStatus();
	}
}

//アクションアイコンの並び順の更新
std::string ScriptWebRunner::remocon_update_elec_action_order(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) const
{
	const std::map<std::string,std::string> request = httpHeaders.getRequest();
	{
		boost::unique_lock<boost::mutex> al(this->updatelock);

		int target_key = -1;
		//elec_ID1_action_ID2_order=hogehoge&elec_ID1_action_ID2_order=hogehoge&elec_ID1_action_ID2_order=hogehoge&elec_ID1_action_ID2_order=hogehoge
		for(auto it = request.begin() ; it != request.end() ; ++it )
		{
			int key1 = -1;
			int key2 = -1;
			sscanf(it->first.c_str(),"elec_%d_action_%d_order" , &key1 , &key2 );
			if (key1 == -1 || key2 == -1)
			{
				continue;
			}
			if (target_key == -1)
			{
				target_key = key1;
			}
			if (key1 != target_key)
			{
				//ふつーは複数の 機材の actionを同時に更新したりはしないはず。
				//とりあえずこれで。
				continue;
			}
			if ( atoi( it->second.c_str() ) <= 0 )
			{
				continue;
			}

			this->PoolMainWindow->Config.Set( it->first.c_str() , it->second.c_str() );
		}

		return this->RemoconStatus();
	}
}

//家電を消す
std::string ScriptWebRunner::remocon_delete_elec(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) const
{
	const std::map<std::string,std::string> request = httpHeaders.getRequest();
	{
		boost::unique_lock<boost::mutex> al(this->updatelock);

		//key=ID
		auto keyIT = request.find("key");
		if (keyIT == request.end() )
		{
			return RemoconStatusError(0,"keyがありません。");
		}
		int key = atoi(keyIT->second.c_str());
		if (key <= 0)
		{
			return RemoconStatusError(0,"keyが数字ではありません。");
		}

		const std::string prefix = std::string("elec_") + num2str(key) + "_";
		const auto configmap = this->PoolMainWindow->Config.FindGetsToMap(prefix,false);
		for(auto it = configmap.begin() ; it != configmap.end() ; ++it )
		{
			this->PoolMainWindow->Config.Remove( it->first.c_str() );
		}

		return this->RemoconStatus();
	}
}

//家電の操作を消す
std::string ScriptWebRunner::remocon_delete_elec_action(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) const
{
	const std::map<std::string,std::string> request = httpHeaders.getRequest();

	{
		boost::unique_lock<boost::mutex> al(this->updatelock);

		//key1=ID&key2=ID
		auto key1IT = request.find("key1");
		if (key1IT == request.end() )
		{
			return RemoconStatusError(0,"key1がありません。");
		}
		int key1 = atoi(key1IT->second.c_str());
		if (key1 <= 0)
		{
			return RemoconStatusError(0,"key1が数字ではありません");
		}

		auto key2IT = request.find("key2");
		if (key2IT == request.end() )
		{
			return RemoconStatusError(0,"key2がありません。");
		}
		int key2 = atoi(key2IT->second.c_str());
		if (key2 <= 0)
		{
			return RemoconStatusError(0,"key2が数字ではありません");
		}

		const std::string prefix = std::string("elec_") + num2str(key1) + "_action_" +  num2str(key2) + "_";
		const auto configmap = this->PoolMainWindow->Config.FindGetsToMap(prefix,false);
		for(auto it = configmap.begin() ; it != configmap.end() ; ++it )
		{
			this->PoolMainWindow->Config.Remove( it->first.c_str() );
		}

		return this->RemoconStatus();
	}
}


//エラーを返す
std::string ScriptWebRunner::RemoconStatusError(int code ,std::string msg)  const
{
	_USE_WINDOWS_ENCODING;

	const std::string jsonstring = std::string("{ status: \"error\" , code: ") + num2str(code)  + ", msg: \"" + XLStringUtil::htmlspecialchars_low(msg) + "\"}";
#ifdef _WINDOWS
	return _A2U(jsonstring.c_str());
#else
	return jsonstring;
#endif
}

//ステータスを jsonで
std::string ScriptWebRunner::RemoconStatus()  const
{
	_USE_WINDOWS_ENCODING;

	//order でソートする必要がある・・・・
	struct sortactionwork
	{
		sortactionwork(int key) : order(9999),key(key){};

		int key;
		int order;
		std::vector<std::pair<std::string,std::string> > arraykeys;
	};
	struct sortwork
	{
		sortwork(int key) : order(9999),key(key){};

		int key;
		int order;
		std::vector<sortactionwork*> action;
		std::vector<std::pair<std::string,std::string> > arraykeys;
	};
	std::vector<sortwork*> sortlist;

	const auto configmap = this->PoolMainWindow->Config.FindGetsToMap("elec_",false);
	for(auto it = configmap.begin() ; it != configmap.end() ; ++it )
	{
		int key = atoi(it->first.c_str() + sizeof("elec_")-1);
		sortwork * p = NULL;
		for(auto findit = sortlist.begin() ; findit != sortlist.end() ; ++findit )
		{
			if ( (*findit)->key == key )
			{
				p = *findit;
				break;
			}
		}
		if (!p)
		{
			p = new sortwork(key);
			sortlist.push_back(p);
		}

		const char * actionpos = strstr(it->first.c_str(),"_action_");
		if (actionpos)
		{
			int actionkey = atoi(actionpos + sizeof("_action_")-1);

			sortactionwork * pp = NULL;
			for(auto findit = p->action.begin() ; findit != p->action.end() ; ++findit )
			{
				if ( (*findit)->key == actionkey )
				{
					pp = *findit;
					break;
				}
			}
			if (!pp)
			{
				pp = new sortactionwork(actionkey);
				p->action.push_back(pp);
			}

			if (strstr(it->first.c_str(),"_order"))
			{
				pp->order = atoi(it->second.c_str());
			}
			pp->arraykeys.push_back(*it);
		}
		else
		{
			if (strstr(it->first.c_str(),"_order"))
			{
				p->order = atoi(it->second.c_str());
			}
			p->arraykeys.push_back(*it);
		}
	}

	//ここでやっとソートする.
	for(auto it = sortlist.begin() ; it != sortlist.end() ; ++it )
	{
		std::sort((*it)->action.begin() , (*it)->action.end() , [](const sortactionwork* a,const sortactionwork* b){
			return a->order < b->order ;
		});
	}
	std::sort(sortlist.begin() , sortlist.end() , [](const sortwork* a,const sortwork* b){
		return a->order < b->order;
	});
	//ソートの結果で出力する文字列を作る
	std::string jsonstring;
	for(auto it = sortlist.begin() ; it != sortlist.end() ; ++it )
	{
		for(auto arrayIT = (*it)->arraykeys.begin() ; arrayIT != (*it)->arraykeys.end() ; ++arrayIT )
		{
			jsonstring += std::string(",") + XLStringUtil::htmlspecialchars_low(arrayIT->first) + ": \"" + XLStringUtil::htmlspecialchars_low(arrayIT->second) + "\"";
		}
		
		for(auto itaction = (*it)->action.begin() ; itaction != (*it)->action.end() ; ++itaction )
		{
			for(auto arrayIT = (*itaction)->arraykeys.begin() ; arrayIT != (*itaction)->arraykeys.end() ; ++arrayIT )
			{
				jsonstring += std::string(",") + XLStringUtil::htmlspecialchars_low(arrayIT->first) + ": \"" + XLStringUtil::htmlspecialchars_low(arrayIT->second) + "\"";
			}
			delete *itaction;
		}
		delete *it;
	}
	jsonstring = std::string("{ status: \"ok\"") + jsonstring + "}";

#ifdef _WINDOWS
	return _A2U(jsonstring.c_str());
#else
	return jsonstring;
#endif
}

int ScriptWebRunner::NewElecID() const
{
	int max = 1;
	const auto configmap = this->PoolMainWindow->Config.FindGetsToMap("elec_",false);
	for(auto it = configmap.begin() ; it != configmap.end() ; ++it )
	{
		const int key = atoi( it->first.c_str() + (sizeof("elec_") - 1) );
		if (key > max)
		{
			max = key;
		}
	}
	return max + 1;
}

int ScriptWebRunner::NewElecActionID(int eleckey) const
{
	int max = 1;
	const std::string prefix = std::string("elec_") + num2str(eleckey) + "_action_";
	const auto configmap = this->PoolMainWindow->Config.FindGetsToMap(prefix,false);
	for(auto it = configmap.begin() ; it != configmap.end() ; ++it )
	{
		const int key = atoi( it->first.c_str() + prefix.size() );
		if (key > max)
		{
			max = key;
		}
	}
	return max + 1;
}

//家電名が重複していないかどうか
bool ScriptWebRunner::checkUniqElecName(int key1,const std::string& name) const
{
	const std::string prefix = std::string("elec_") ;
	const auto configmap = this->PoolMainWindow->Config.FindGetsToMap(prefix,false);
	for(auto it = configmap.begin() ; it != configmap.end() ; ++it )
	{
		const int key = atoi( it->first.c_str() + prefix.size() );
		if (key == key1)
		{//自分自身と重複している分にはいいわけで
			continue;
		}
		if (!strstr(it->first.c_str(),"_name")  )
		{
			continue;
		}
		if (strstr(it->first.c_str(),"_action_")  )
		{
			continue;
		}
		if ( it->second == name )
		{
			return false;
		}
	}
	return true;
}

//家電操作名が重複していないかどうか
bool ScriptWebRunner::checkUniqElecActionName(int key1,int key2,const std::string& name) const
{
	const std::string prefix = std::string("elec_") + num2str(key1) + "_action_";
	const auto configmap = this->PoolMainWindow->Config.FindGetsToMap(prefix,false);
	for(auto it = configmap.begin() ; it != configmap.end() ; ++it )
	{
		const int key = atoi( it->first.c_str() + prefix.size() );
		if (key == key2)
		{//自分自身と重複している分にはいいわけで
			continue;
		}
		if (!strstr(it->first.c_str(),"_name"))
		{
			continue;
		}
		if ( it->second == name )
		{
			return false;
		}
	}
	return true;
}

bool ScriptWebRunner::checkUniqElecType(int key,const std::string& type,const std::string& type_other) const
{
	return true;
}

bool ScriptWebRunner::checkUniqElecActionType(int key1,int key2,const std::string& actiontype,const std::string& actiontype_other) const
{
	return true;
}

//新しい機材の並び順を求める(末尾に追加する)
int ScriptWebRunner::newOrderElec() const
{
	int max = 0;
	const std::string prefix = std::string("elec_") ;
	const auto configmap = this->PoolMainWindow->Config.FindGetsToMap(prefix,false);
	for(auto it = configmap.begin() ; it != configmap.end() ; ++it )
	{
		if (!strstr(it->first.c_str(),"_order")  )
		{
			continue;
		}
		if (strstr(it->first.c_str(),"_action_")  )
		{
			continue;
		}
		const int order = atoi(it->second.c_str());
		if (max < order ) max = order;
	}
	return max+1;
}

//新しい機材操作の並び順を求める(末尾に追加する)
int ScriptWebRunner::newOrderElecAction(int key1) const
{
	int max = 0;
	const std::string prefix = std::string("elec_") + num2str(key1) + "_action_";
	const auto configmap = this->PoolMainWindow->Config.FindGetsToMap(prefix,false);
	for(auto it = configmap.begin() ; it != configmap.end() ; ++it )
	{
		if (!strstr(it->first.c_str(),"_order"))
		{
			continue;
		}
		const int order = atoi(it->second.c_str());
		if (max < order ) max = order;
	}
	return max+1;
}

//機材のアップデート
//editable_key=123&name=名前&type=タイプ&type_other=タイプその他
std::string ScriptWebRunner::remocon_update_elec(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) const
{
	const std::map<std::string,std::string> request = httpHeaders.getRequest();
	{
		boost::unique_lock<boost::mutex> al(this->updatelock);

		const std::string editable_key = mapfind(request,"editable_key");

		int key;
		if (editable_key == "new")
		{//家電の新しいIDを降り出す
			key = NewElecID();
		}
		else
		{
			key = atoi(editable_key.c_str());
		}
		if (key <= 0) 
		{
			return RemoconStatusError(10000 , "editable_keyの値が数字ではありません");
		}
		const auto prefix = std::string("elec_") + num2str(key)  ;

		const std::string name = mapfind(request,"name");
		if (! checkUniqElecName(key,name) )
		{
			return RemoconStatusError(10010 , "nameで設定しようとしている操作名称は既に使われています");
		}

		const std::string type = mapfind(request,"type");
		const std::string type_other = mapfind(request,"type_other");
		if (! checkUniqElecType(key,type,type_other) )
		{
			return RemoconStatusError(10020 , "typeで設定しようとしている操作種類は既に使われています");
		}

		this->PoolMainWindow->Config.Set( prefix + "_name" , name );
		this->PoolMainWindow->Config.Set( prefix + "_type" , type );
		this->PoolMainWindow->Config.Set( prefix + "_type_other" , type_other );

		if (editable_key == "new")
		{
			this->PoolMainWindow->Config.Set( prefix + "_status" , "" );
			this->PoolMainWindow->Config.Set( prefix + "_order" , num2str( newOrderElec() ) );
		}		
	}
	return this->RemoconStatus();
}

//機材のアクション項目のアップデート
//actioneditable_key1=123&actioneditable_key2=456&actionname=名前&actiontype=種類&actiontype_other=種類その他&actionvoice=1&actionvoice_command=こんぴゅーた&showremocon=1&useapi=1&useinternet=1&tospeak_select=ターゲット&tospeak_string=読み上げ文字列&tospeak_mp3=hoge.mp3&actionexecuteflag=赤外線&actionexecuteflag_ir=12345678910&actionexecuteflag_command=a.exe&actionexecuteflag_command_args1=&actionexecuteflag_command_args2=&actionexecuteflag_command_args3=&actionexecuteflag_command_args4=&actionexecuteflag_command_args5=
std::string ScriptWebRunner::remocon_update_elec_action(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) const
{
	const std::map<std::string,std::string> request = httpHeaders.getRequest();
	{
		boost::unique_lock<boost::mutex> al(this->updatelock);

		const std::string actioneditable_key1 = mapfind(request,"actioneditable_key1");
		int key1 = atoi(actioneditable_key1.c_str());
		if (key1 <= 0) 
		{
			return RemoconStatusError(20002 , "actioneditable_key1の値が数字ではありません");
		}

		const std::string actioneditable_key2 = mapfind(request,"actioneditable_key2");
		int key2;
		if (actioneditable_key2 == "new")
		{//家電操作の新しいIDを降り出す
			key2 = NewElecActionID(key1);
		}
		else
		{
			key2 = atoi(actioneditable_key2.c_str());
		}
		if (key2 <= 0) 
		{
			return RemoconStatusError(20004 , "actioneditable_key2の値が数字ではありません");
		}
		const auto prefix = std::string("elec_") + num2str(key1) + "_action_" + num2str(key2) ;


		const std::string actionname = mapfind(request,"actionname");
		if (! checkUniqElecActionName(key1,key2,actionname) )
		{
			return RemoconStatusError(20005 , "actionnameで設定しようとしている操作名称は既に使われています");
		}

		const std::string actiontype = mapfind(request,"actiontype");
		const std::string actiontype_other = mapfind(request,"actiontype_other");
		if (! checkUniqElecActionType(key1,key2,actiontype,actiontype_other) )
		{
			return RemoconStatusError(20005 , "actionnameで設定しようとしている操作名称は既に使われています");
		}

		const std::string actionvoice = mapfind(request,"actionvoice");
		const std::string actionvoice_command = mapfind(request,"actionvoice_command");
		const std::string showremocon = mapfind(request,"showremocon");
		const std::string useapi = mapfind(request,"useapi");
		const std::string useinternet = mapfind(request,"useinternet");
		const std::string tospeak = mapfind(request,"tospeak");
		const std::string tospeak_select = mapfind(request,"tospeak_select");
		const std::string tospeak_string = mapfind(request,"tospeak_string");
		const std::string tospeak_mp3 = mapfind(request,"tospeak_mp3");
		const std::string actionexecuteflag = mapfind(request,"actionexecuteflag");
		const std::string actionexecuteflag_ir = mapfind(request,"actionexecuteflag_ir");
		const std::string actionexecuteflag_command = mapfind(request,"actionexecuteflag_command");
		const std::string actionexecuteflag_command_args1 = mapfind(request,"actionexecuteflag_command_args1");
		const std::string actionexecuteflag_command_args2 = mapfind(request,"actionexecuteflag_command_args2");
		const std::string actionexecuteflag_command_args3 = mapfind(request,"actionexecuteflag_command_args3");
		const std::string actionexecuteflag_command_args4 = mapfind(request,"actionexecuteflag_command_args4");
		const std::string actionexecuteflag_command_args5 = mapfind(request,"actionexecuteflag_command_args5");

		this->PoolMainWindow->Config.Set( prefix + "_actionname" , actionname );
		this->PoolMainWindow->Config.Set( prefix + "_actiontype" , actiontype );
		this->PoolMainWindow->Config.Set( prefix + "_actiontype_other" , actiontype_other );
		this->PoolMainWindow->Config.Set( prefix + "_actionvoice" , actionvoice );
		this->PoolMainWindow->Config.Set( prefix + "_actionvoice_command" , actionvoice_command );
		this->PoolMainWindow->Config.Set( prefix + "_showremocon" , showremocon );
		this->PoolMainWindow->Config.Set( prefix + "_useapi" , useapi );
		this->PoolMainWindow->Config.Set( prefix + "_useinternet" , useinternet );
		this->PoolMainWindow->Config.Set( prefix + "_topeak" , tospeak );
		this->PoolMainWindow->Config.Set( prefix + "_tospeak_select" , tospeak_select );
		this->PoolMainWindow->Config.Set( prefix + "_tospeak_string" , tospeak_string );
		this->PoolMainWindow->Config.Set( prefix + "_tospeak_mp3" , tospeak_mp3 );
		this->PoolMainWindow->Config.Set( prefix + "_actionexecuteflag" , actionexecuteflag );
		this->PoolMainWindow->Config.Set( prefix + "_actionexecuteflag_ir" , actionexecuteflag_ir );
		this->PoolMainWindow->Config.Set( prefix + "_actionexecuteflag_command" , actionexecuteflag_command );
		this->PoolMainWindow->Config.Set( prefix + "_actionexecuteflag_command_args1" , actionexecuteflag_command_args1 );
		this->PoolMainWindow->Config.Set( prefix + "_actionexecuteflag_command_args2" , actionexecuteflag_command_args2 );
		this->PoolMainWindow->Config.Set( prefix + "_actionexecuteflag_command_args3" , actionexecuteflag_command_args3 );
		this->PoolMainWindow->Config.Set( prefix + "_actionexecuteflag_command_args4" , actionexecuteflag_command_args4 );
		this->PoolMainWindow->Config.Set( prefix + "_actionexecuteflag_command_args5" , actionexecuteflag_command_args5 );
		if (actioneditable_key2 == "new")
		{
			this->PoolMainWindow->Config.Set( prefix + "_order" , num2str(newOrderElecAction(key1)) );
		}
	}

	return this->RemoconStatus();
}


//リファラーをチェックします。
bool ScriptWebRunner::checkReferer(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result,std::string* responsString) const
{
	//内部からのリファラーを持つアクセスだったら許可する。
	//理由:ブラウザからのXSSを防止するのが目的なのでこれで大丈夫だと思われる
	const auto header = httpHeaders.getHeader();
	auto it = header.find("Referer");
	if (it == header.end())
	{
		it = header.find("referer");
	}
	if (it != header.end())
	{
		if ( it->second.find( this->PoolMainWindow->Httpd.getServerTop() ) == 0 )
		{
			//OK 内部からのアクセスだ。
			return true;
		}
	}

	//XSSいたずら防止のためにリファラーを求める.
	*result = WEBSERVER_RESULT_TYPE_FORBIDDEN;
	*responsString = RemoconStatusError(9000,"変更が発生するリクエストには、FutureHomeControllerからのリファラーが必要です。");
	return false;
}

//webからアクセスがあったときに呼ばれます。
bool ScriptWebRunner::WebAccess(const std::string& path,const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result,std::string* responsString) const
{
	ASSERT___IS_WORKER_THREAD_RUNNING(); //メインスレッド以外で動きます。

	if (path == "/remocon/")
	{
		*result = WEBSERVER_RESULT_TYPE_OK;
		*responsString = RemoconIndex(httpHeaders,result);
		return true;
	}
	else if (path == "/remocon/get/status") 
	{
		*result = WEBSERVER_RESULT_TYPE_OK;
		*responsString = RemoconStatus();
		return true;
	}
	else if (path == "/remocon/update/icon_order") 
	{
		if (!checkReferer(httpHeaders,result,responsString))
		{
			return true;
		}
		*result = WEBSERVER_RESULT_TYPE_OK;
		*responsString = remocon_update_icon_order(httpHeaders,result);
		return true;
	}
	else if (path == "/remocon/update/elec") 
	{
		if (!checkReferer(httpHeaders,result,responsString))
		{
			return true;
		}
		*result = WEBSERVER_RESULT_TYPE_OK;
		*responsString = remocon_update_elec(httpHeaders,result);
		return true;
	}
	else if (path == "/remocon/update/elec_action") 
	{
		if (!checkReferer(httpHeaders,result,responsString))
		{
			return true;
		}
		*result = WEBSERVER_RESULT_TYPE_OK;
		*responsString = remocon_update_elec_action(httpHeaders,result);
		return true;
	}
	else if (path == "/remocon/update/elec_action_order") 
	{
		if (!checkReferer(httpHeaders,result,responsString))
		{
			return true;
		}
		*result = WEBSERVER_RESULT_TYPE_OK;
		*responsString = remocon_update_elec_action_order(httpHeaders,result);
		return true;
	}
	else if (path == "/remocon/delete/elec") 
	{
		if (!checkReferer(httpHeaders,result,responsString))
		{
			return true;
		}
		*result = WEBSERVER_RESULT_TYPE_OK;
		*responsString = remocon_delete_elec(httpHeaders,result);
		return true;
	}
	else if (path == "/remocon/delete/elec_action") 
	{
		if (!checkReferer(httpHeaders,result,responsString))
		{
			return true;
		}
		*result = WEBSERVER_RESULT_TYPE_OK;
		*responsString = remocon_delete_elec_action(httpHeaders,result);
		return true;
	}
	else if (path == "/remocon/fire/byid") 
	{
		if (!checkReferer(httpHeaders,result,responsString))
		{
			return true;
		}
		*result = WEBSERVER_RESULT_TYPE_OK;
		*responsString = remocon_fire_byid(httpHeaders,result);
		return true;
	}
	else if (path == "/remocon/fire/byname") 
	{
		if (!checkReferer(httpHeaders,result,responsString))
		{
			return true;
		}
		*result = WEBSERVER_RESULT_TYPE_OK;
		*responsString = remocon_fire_byname(httpHeaders,result);
		return true;
	}
	else
	{
		return false;
	}
}