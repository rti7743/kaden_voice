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

bool ScriptWebRunner::Load()
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
			if (commandIT->second.empty())
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

//インスタンスの再読み込み
//プログラムをデバッグしているときとか、再読み込み機能がないと死ねるから・・・
bool ScriptWebRunner::Reload()
{
	this->PoolMainWindow->SyncInvoke( [=](){
		for(auto it = this->callbackHistoryList.begin() ; it != this->callbackHistoryList.end() ; ++it)
		{
			if (this->PoolMainWindow->Recognition.RemoveCallback(*it,false))
			{
				break;	
			}
			if (this->PoolMainWindow->Speak.RemoveCallback(*it,false))
			{
				break;	
			}
		}
		Load();

		this->PoolMainWindow->SyncInvokeLog("音声認識エンジンコミット開始",LOG_LEVEL_DEBUG);
		this->PoolMainWindow->Recognition.CommitRule();
		this->PoolMainWindow->SyncInvokeLog("音声認識エンジンコミット終了",LOG_LEVEL_DEBUG);
		return ;
	} );

	return true;
}

std::string ScriptWebRunner::callbackFunction(const CallbackDataStruct* callback,const std::map<std::string , std::string> & match)
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
			runner.callFunction("call",args,true);
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
std::string ScriptWebRunner::remocon_fire_bytype(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) const
{
	const std::map<std::string,std::string> request = httpHeaders.getRequest();
	{
		//type1=hogehoge&type2=hogehoge
		auto type1IT = request.find("type1");
		if (type1IT == request.end())
		{
			return RemoconStatusError(20001 , "type1が見つかりません");
		}

		auto type2IT = request.find("type2");
		if (type2IT == request.end())
		{
			return RemoconStatusError(20003 , "type2が見つかりません");
		}

		int key1 = -1;
		const auto configmap = this->PoolMainWindow->Config.FindGetsToMap("elec_",false);
		for(auto it = configmap.begin() ; it != configmap.end() ; ++it )
		{
			if ( it->second == type1IT->second )
			{
				if ( ! strstr( it->first.c_str() , "_type" ) && !strstr( it->first.c_str() , "_action_" ) )
				{
					continue;
				}
				sscanf(it->first.c_str(),"elec_%d_type",&key1);
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
			if ( it->second == type2IT->second )
			{
				if ( ! strstr( it->first.c_str() , "_type" ) && strstr( it->first.c_str() , prefix.c_str() ) )
				{
					continue;
				}
				sscanf(it->first.c_str(),"elec_%d_action_%d_type",&key1,&key2);
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
std::string ScriptWebRunner::remocon_delete_elec(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) 
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

		Reload();

		return this->RemoconStatus();
	}
}

//家電の操作を消す
std::string ScriptWebRunner::remocon_delete_elec_action(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result)
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

		Reload();

		return this->RemoconStatus();
	}
}


//エラーを返す
std::string ScriptWebRunner::RemoconStatusError(int code ,std::string msg)  const
{
	_USE_WINDOWS_ENCODING;

	const std::string jsonstring = std::string("{ \"status\": \"error\" , \"code\": ") + num2str(code)  + ", \"message\": \"" + XLStringUtil::htmlspecialchars_low(msg) + "\"}";
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
			jsonstring += std::string(",\"") + XLStringUtil::htmlspecialchars_low(arrayIT->first) + "\": \"" + XLStringUtil::htmlspecialchars_low(arrayIT->second) + "\"";
		}
		
		for(auto itaction = (*it)->action.begin() ; itaction != (*it)->action.end() ; ++itaction )
		{
			for(auto arrayIT = (*itaction)->arraykeys.begin() ; arrayIT != (*itaction)->arraykeys.end() ; ++arrayIT )
			{
				jsonstring += std::string(",\"") + XLStringUtil::htmlspecialchars_low(arrayIT->first) + "\": \"" + XLStringUtil::htmlspecialchars_low(arrayIT->second) + "\"";
			}
			delete *itaction;
		}
		delete *it;
	}
	jsonstring = std::string("{ \"status\": \"ok\" ") + jsonstring + "}";

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

//機材の種類が重複していたらエラー
bool ScriptWebRunner::checkUniqElecType(int key1,const std::string& type) const
{
	const std::string prefix = std::string("elec_") ;
	const auto configmap = this->PoolMainWindow->Config.FindGetsToMap(prefix,false);

	//その他があるのでめんどい所
	for(auto it = configmap.begin() ; it != configmap.end() ; ++it )
	{
		const int targetkey = atoi( it->first.c_str() + prefix.size() );
		if (targetkey == key1)
		{//自分自身と重複している分にはいいわけで
			continue;
		}
		if (!strstr(it->first.c_str(),"_type")  )
		{
			continue;
		}
		if (strstr(it->first.c_str(),"_action_")  )
		{
			continue;
		}

		if (it->second == type)
		{//既に使われている!
			return false;
		}
	}
	return true;
}

//機材操作の種類が重複していたらエラー
bool ScriptWebRunner::checkUniqElecActionType(int key1,int key2,const std::string& actiontype) const
{
	const std::string prefix = std::string("elec_") + num2str(key1) + "_action_";
	const auto configmap = this->PoolMainWindow->Config.FindGetsToMap(prefix,false);

	//その他があるのでめんどい所
	for(auto it = configmap.begin() ; it != configmap.end() ; ++it )
	{
		const int targetkey = atoi( it->first.c_str() + prefix.size() );
		if (targetkey == key2)
		{//自分自身と重複している分にはいいわけで
			continue;
		}
		if (!strstr(it->first.c_str(),"_actiontype")  )
		{
			continue;
		}

		if (it->second == actiontype)
		{//既に使われている!
			return false;
		}
	}
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
//editable_key=123&type=タイプ&type_other=タイプその他
std::string ScriptWebRunner::remocon_update_elec(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) 
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

		std::string type = mapfind(request,"type");
		if (type == "" || type == "その他") type = mapfind(request,"type_other");
		if (! checkUniqElecType(key,type) )
		{
			return RemoconStatusError(10020 , "typeで設定しようとしている操作種類は既に使われています");
		}
		const std::string elecicon = mapfind(request,"elecicon");

		this->PoolMainWindow->Config.Set( prefix + "_type" , type );
		this->PoolMainWindow->Config.Set( prefix + "_elecicon" , elecicon );

		if (editable_key == "new")
		{
			this->PoolMainWindow->Config.Set( prefix + "_status" , "" );
			this->PoolMainWindow->Config.Set( prefix + "_order" , num2str( newOrderElec() ) );
		}

//		Reload();
	}
	return this->RemoconStatus();
}

//機材のアクション項目のアップデート
//actioneditable_key1=123&actioneditable_key2=456&actiontype=種類&actiontype_other=種類その他&actionvoice=1&actionvoice_command=こんぴゅーた&showremocon=1&useapi=1&useinternet=1&tospeak_select=ターゲット&tospeak_string=読み上げ文字列&tospeak_mp3=hoge.mp3&actionexecuteflag=赤外線&actionexecuteflag_ir=12345678910&actionexecuteflag_command=a.exe&actionexecuteflag_command_args1=&actionexecuteflag_command_args2=&actionexecuteflag_command_args3=&actionexecuteflag_command_args4=&actionexecuteflag_command_args5=
std::string ScriptWebRunner::remocon_update_elec_action(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) 
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

		std::string actiontype = mapfind(request,"actiontype");
		if (actiontype == "" || actiontype == "その他") actiontype = mapfind(request,"actiontype_other");

		if (! checkUniqElecActionType(key1,key2,actiontype) )
		{
			return RemoconStatusError(20005 , "actiontypeで設定しようとしている操作名称は既に使われています");
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

		const std::string old_actionvoice = this->PoolMainWindow->Config.Get( prefix + "_actionvoice","");
		const std::string old_actionvoice_command = this->PoolMainWindow->Config.Get( prefix + "_actionvoice_command","");

		this->PoolMainWindow->Config.Set( prefix + "_actiontype" , actiontype );
		this->PoolMainWindow->Config.Set( prefix + "_actionvoice" , actionvoice );
		this->PoolMainWindow->Config.Set( prefix + "_actionvoice_command" , actionvoice_command );
		this->PoolMainWindow->Config.Set( prefix + "_showremocon" , showremocon );
		this->PoolMainWindow->Config.Set( prefix + "_useapi" , useapi );
		this->PoolMainWindow->Config.Set( prefix + "_useinternet" , useinternet );
		this->PoolMainWindow->Config.Set( prefix + "_tospeak" , tospeak );
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

		bool is_reaload = false;
		if (actioneditable_key2 == "new")
		{
			this->PoolMainWindow->Config.Set( prefix + "_order" , num2str(newOrderElecAction(key1)) );
			is_reaload = true;
		}
		else
		{
			if (old_actionvoice != actionvoice || old_actionvoice_command != actionvoice_command )
			{
				is_reaload = true;
			}
		}

		//リロード処理
		if (is_reaload)
		{
			Reload();
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

//設定のweb index
std::string ScriptWebRunner::SettingIndex(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) const
{
	const std::string filename = this->PoolMainWindow->Httpd.WebPathToRealPath("setting.tpl");
	const std::string html = XLFileUtil::cat(filename);

	return XLStringUtil::replace_low(html,"%SETTING%", SettingStatus() );
}

//設定のステータスを jsonで
std::string ScriptWebRunner::SettingStatus()  const
{
	_USE_WINDOWS_ENCODING;

	const auto configmap = this->PoolMainWindow->Config.FindGetsToMap("setting_",false);
	std::string jsonstring;
	for(auto it = configmap.begin() ; it != configmap.end() ; ++it )
	{
		jsonstring += std::string(",\"") + XLStringUtil::htmlspecialchars_low(it->first) + "\": " + "\"" + XLStringUtil::htmlspecialchars_low(it->second) + "\"";
	}

	jsonstring = std::string("{ \"status\": \"ok\" ") + jsonstring + "}";

#ifdef _WINDOWS
	return _A2U(jsonstring.c_str());
#else
	return jsonstring;
#endif
}

std::string ScriptWebRunner::setting_update_setting_account(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) const
{
	const std::map<std::string,std::string> request = httpHeaders.getRequest();
	{
		boost::unique_lock<boost::mutex> al(this->updatelock);

		const std::string setting_account_usermail = mapfind(request,"setting_account_usermail");
		const std::string setting_account_password = mapfind(request,"setting_account_password");

		this->PoolMainWindow->Config.Set( "setting_account_usermail" , setting_account_usermail );
		if (setting_account_password != "<%PASSWORD NO CHANGE%>")
		{
			this->PoolMainWindow->Config.Set( "setting_account_password" , setting_account_password );
		}
	}

	return this->SettingStatus();
}

std::string ScriptWebRunner::setting_update_setting_network(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) const
{
	const std::map<std::string,std::string> request = httpHeaders.getRequest();
	{
		boost::unique_lock<boost::mutex> al(this->updatelock);

		const std::string setting_network_ipaddress_type = mapfind(request,"setting_network_ipaddress_type");
		const std::string setting_network_ipaddress_type_fixed_ip = mapfind(request,"setting_network_ipaddress_type_fixed_ip");
		const std::string setting_network_ipaddress_type_fixed_mask = mapfind(request,"setting_network_ipaddress_type_fixed_mask");
		const std::string setting_network_ipaddress_type_fixed_gateway = mapfind(request,"setting_network_ipaddress_type_fixed_gateway");
		const std::string setting_network_ipaddress_type_fixed_dns = mapfind(request,"setting_network_ipaddress_type_fixed_dns");

		this->PoolMainWindow->Config.Set( "setting_network_ipaddress_type" , setting_network_ipaddress_type );
		this->PoolMainWindow->Config.Set( "setting_network_ipaddress_type_fixed_ip" , setting_network_ipaddress_type_fixed_ip );
		this->PoolMainWindow->Config.Set( "setting_network_ipaddress_type_fixed_mask" , setting_network_ipaddress_type_fixed_mask );
		this->PoolMainWindow->Config.Set( "setting_network_ipaddress_type_fixed_gateway" , setting_network_ipaddress_type_fixed_gateway );
		this->PoolMainWindow->Config.Set( "setting_network_ipaddress_type_fixed_dns" , setting_network_ipaddress_type_fixed_dns );
	}

	return this->SettingStatus();
}

std::string ScriptWebRunner::setting_update_setting_recong(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) const
{
	const std::map<std::string,std::string> request = httpHeaders.getRequest();
	{
		boost::unique_lock<boost::mutex> al(this->updatelock);

		const std::string setting_recong_recong_type = mapfind(request,"setting_recong_recong_type");
		const std::string setting_recong_recong_volume = mapfind(request,"setting_recong_recong_volume");
//		const std::string setting_recong_reco__yobikake__1 = mapfind(request,"setting_recong_reco__yobikake__1");
//		const std::string setting_recong_reco__yobikake__2 = mapfind(request,"setting_recong_reco__yobikake__2");
//		const std::string setting_recong_reco__yobikake__3 = mapfind(request,"setting_recong_reco__yobikake__3");
//		const std::string setting_recong_reco__yobikake__4 = mapfind(request,"setting_recong_reco__yobikake__4");
//		const std::string setting_recong_reco__yobikake__5 = mapfind(request,"setting_recong_reco__yobikake__5");

		this->PoolMainWindow->Config.Set( "setting_recong_recong_type" , setting_recong_recong_type );
		this->PoolMainWindow->Config.Set( "setting_recong_recong_volume" , setting_recong_recong_volume );
//		this->PoolMainWindow->Config.Set( "setting_recong_reco__yobikake__1" , setting_recong_reco__yobikake__1 );
//		this->PoolMainWindow->Config.Set( "setting_recong_reco__yobikake__2" , setting_recong_reco__yobikake__2 );
//		this->PoolMainWindow->Config.Set( "setting_recong_reco__yobikake__3" , setting_recong_reco__yobikake__3 );
//		this->PoolMainWindow->Config.Set( "setting_recong_reco__yobikake__4" , setting_recong_reco__yobikake__4 );
//		this->PoolMainWindow->Config.Set( "setting_recong_reco__yobikake__5" , setting_recong_reco__yobikake__5 );
	}

	return this->SettingStatus();
}

std::string ScriptWebRunner::setting_update_setting_speak(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) const
{
	const std::map<std::string,std::string> request = httpHeaders.getRequest();
	{
		boost::unique_lock<boost::mutex> al(this->updatelock);

		const std::string setting_speak_speak_type = mapfind(request,"setting_speak_speak_type");
		const std::string setting_speak_speak_volume = mapfind(request,"setting_speak_speak_volume");

		this->PoolMainWindow->Config.Set( "setting_speak_speak_type" , setting_speak_speak_type );
		this->PoolMainWindow->Config.Set( "setting_speak_speak_volume" , setting_speak_speak_volume );
	}

	return this->SettingStatus();
}

std::string ScriptWebRunner::remocon_fileselectpage_find(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) const
{
	_USE_WINDOWS_ENCODING;
	const std::map<std::string,std::string> request = httpHeaders.getRequest();
	const std::string typepath = mapfind(request,"typepath");
	const std::string search = mapfind(request,"search");

	std::string path = "";
	if (typepath == "tospeak_mp3")
	{
		path = "./webroot/user/tospeak_mp3/";
	}
	else if (typepath == "elecicon")
	{
		path = "./webroot/user/elecicon/";
	}
	else
	{
		return RemoconStatusError(20005 , "typepathが正しくありません");
	}
	std::string jsonstring;

	int fileid = 1;
	const std::string baseDirectory = XLStringUtil::pathcombine( this->PoolMainWindow->Config.GetBaseDirectory() , path);
	bool ret = XLFileUtil::findfile(baseDirectory , [&](const std::string& filename,const std::string& fullfilename) -> bool {
		
		if (filename == "." || filename == "..")
		{
			return true;
		}
		if (!search.empty())
		{
			if ( ! XLStringUtil::stristr(filename,search) )
			{
				return true;
			}
		}
		const std::string prefix = std::string("file_") + num2str(fileid) ;

		std::string icon;
		if (typepath == "tospeak_mp3")
		{
			icon = "./jscss/images/icon_audio.png";
		}
		else if (typepath == "elecicon")
		{
			icon = std::string("./user/elecicon/") + filename;
		}
		
		jsonstring += std::string(",\"") + XLStringUtil::htmlspecialchars_low(prefix + "_name") + "\": " + "\"" + XLStringUtil::htmlspecialchars_low(filename) + "\""
			+ std::string(",\"") + XLStringUtil::htmlspecialchars_low(prefix + "_size") + "\": " + "\"" + XLStringUtil::htmlspecialchars_low(num2str(XLFileUtil::getfilesize(fullfilename) )) + "\""
			+ std::string(",\"") + XLStringUtil::htmlspecialchars_low(prefix + "_time") + "\": " + "\"" + XLStringUtil::htmlspecialchars_low(num2str(XLFileUtil::getfiletime(fullfilename) )) + "\""
			+ std::string(",\"") + XLStringUtil::htmlspecialchars_low(prefix + "_date") + "\": " + "\"" + XLStringUtil::htmlspecialchars_low(XLStringUtil::timetostr(XLFileUtil::getfiletime(fullfilename),"%Y/%m/%d %H:%M:%S" )) + "\""
			+ std::string(",\"") + XLStringUtil::htmlspecialchars_low(prefix + "_icon") + "\": " + "\"" + XLStringUtil::htmlspecialchars_low(icon) + "\""
		;
		fileid++;
		return true;
	});

	jsonstring = std::string("{ \"status\": \"ok\" ") + jsonstring + "}";

#ifdef _WINDOWS
	return _A2U(jsonstring.c_str());
#else
	return jsonstring;
#endif
}

std::string ScriptWebRunner::remocon_fileselectpage_upload(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) const
{
	_USE_WINDOWS_ENCODING;
	const std::map<std::string,std::string> request = httpHeaders.getRequest();
	const std::string typepath = mapfind(request,"typepath");

	std::string path = "";
	if (typepath == "tospeak_mp3")
	{
		path = XLStringUtil::pathcombine(this->PoolMainWindow->Config.GetBaseDirectory() , "./webroot/user/tospeak_mp3/");
	}
	else if (typepath == "elecicon")
	{
		path = XLStringUtil::pathcombine(this->PoolMainWindow->Config.GetBaseDirectory() , "./webroot/user/elecicon/");
	}
	else
	{
		return RemoconStatusError(20005 , "typepathが正しくありません");
	}

	auto files = httpHeaders.getFilesPointer();
	for(auto it = files->begin() ; it != files->end() ; ++ it )
	{
		const std::string savepath = XLStringUtil::pathcombine( path , it->second->filename);
		XLFileUtil::write(savepath , it->second->data);
	}

	return remocon_fileselectpage_find(httpHeaders,result);
}


std::string ScriptWebRunner::remocon_fileselectpage_delete(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) const
{
	_USE_WINDOWS_ENCODING;
	const std::map<std::string,std::string> request = httpHeaders.getRequest();
	const std::string typepath = mapfind(request,"typepath");
	const std::string filter = mapfind(request,"filter");

	std::string path = "";
	if (typepath == "tospeak_mp3")
	{
		path = "./user/tospeak_mp3/";
	}
	else if (typepath == "elecicon")
	{
		path = "./user/elecicon/";
	}
	else
	{
		return RemoconStatusError(20005 , "typepathが正しくありません");
	}
	std::string jsonstring;
	jsonstring = std::string("{ \"status\": \"ok\" ") + jsonstring + "}";

#ifdef _WINDOWS
	return _A2U(jsonstring.c_str());
#else
	return jsonstring;
#endif
}

//音声認識エンジンをアップデートする
std::string ScriptWebRunner::remocon_recongupdate(const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result) const
{
	return "";
}


//webからアクセスがあったときに呼ばれます。
bool ScriptWebRunner::WebAccess(const std::string& path,const XLHttpHeader& httpHeaders,WEBSERVER_RESULT_TYPE* result,std::string* responsString)
{
	ASSERT___IS_WORKER_THREAD_RUNNING(); //メインスレッド以外で動きます。

	if (path == "/remocon/")
	{
		*result = WEBSERVER_RESULT_TYPE_OK_HTML;
		*responsString = RemoconIndex(httpHeaders,result);
		return true;
	}
	else if (path == "/remocon/get/status") 
	{
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = RemoconStatus();
		return true;
	}
	else if (path == "/remocon/update/icon_order") 
	{
		if (!checkReferer(httpHeaders,result,responsString))
		{
			return true;
		}
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = remocon_update_icon_order(httpHeaders,result);
		return true;
	}
	else if (path == "/remocon/update/elec") 
	{
		if (!checkReferer(httpHeaders,result,responsString))
		{
			return true;
		}
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = remocon_update_elec(httpHeaders,result);
		return true;
	}
	else if (path == "/remocon/update/elec_action") 
	{
		if (!checkReferer(httpHeaders,result,responsString))
		{
			return true;
		}
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = remocon_update_elec_action(httpHeaders,result);
		return true;
	}
	else if (path == "/remocon/update/elec_action_order") 
	{
		if (!checkReferer(httpHeaders,result,responsString))
		{
			return true;
		}
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = remocon_update_elec_action_order(httpHeaders,result);
		return true;
	}
	else if (path == "/remocon/delete/elec") 
	{
		if (!checkReferer(httpHeaders,result,responsString))
		{
			return true;
		}
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = remocon_delete_elec(httpHeaders,result);
		return true;
	}
	else if (path == "/remocon/delete/elec_action") 
	{
		if (!checkReferer(httpHeaders,result,responsString))
		{
			return true;
		}
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = remocon_delete_elec_action(httpHeaders,result);
		return true;
	}
	else if (path == "/remocon/fire/byid") 
	{
		if (!checkReferer(httpHeaders,result,responsString))
		{
			return true;
		}
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = remocon_fire_byid(httpHeaders,result);
		return true;
	}
	else if (path == "/remocon/fire/bytype") 
	{
		if (!checkReferer(httpHeaders,result,responsString))
		{
			return true;
		}
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = remocon_fire_bytype(httpHeaders,result);
		return true;
	}
	else if (path == "/setting/")
	{
		*result = WEBSERVER_RESULT_TYPE_OK_HTML;
		*responsString = SettingIndex(httpHeaders,result);
		return true;
	}
	else if (path == "/setting/get/status") 
	{
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = SettingStatus();
		return true;
	}
	else if (path == "/setting/update/setting_account") 
	{
		if (!checkReferer(httpHeaders,result,responsString))
		{
			return true;
		}
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = setting_update_setting_account(httpHeaders,result);
		return true;
	}
	else if (path == "/setting/update/setting_network") 
	{
		if (!checkReferer(httpHeaders,result,responsString))
		{
			return true;
		}
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = setting_update_setting_network(httpHeaders,result);
		return true;
	}
	else if (path == "/setting/update/setting_recong") 
	{
		if (!checkReferer(httpHeaders,result,responsString))
		{
			return true;
		}
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = setting_update_setting_recong(httpHeaders,result);
		return true;
	}
	else if (path == "/setting/update/setting_speak") 
	{
		if (!checkReferer(httpHeaders,result,responsString))
		{
			return true;
		}
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = setting_update_setting_speak(httpHeaders,result);
		return true;
	}
	else if (path == "/remocon/fileselectpage/find") 
	{
		if (!checkReferer(httpHeaders,result,responsString))
		{
			return true;
		}
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = remocon_fileselectpage_find(httpHeaders,result);
		return true;
	}
	else if (path == "/remocon/fileselectpage/upload") 
	{
		if (!checkReferer(httpHeaders,result,responsString))
		{
			return true;
		}
		*result = WEBSERVER_RESULT_TYPE_OK_JSON;
		*responsString = remocon_fileselectpage_upload(httpHeaders,result);
		return true;
	}
	else
	{
		return false;
	}
}