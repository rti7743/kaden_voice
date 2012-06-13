#include "common.h"
#include "ScriptRunner.h"
#include "WebMenu.h"
#include "MainWindow.h"
#include "XLStringUtil.h"

WebMenu::WebMenu()
{
}
WebMenu::~WebMenu()
{
	for(auto roomIT = this->rooms.begin() ; roomIT != this->rooms.end() ; ++ roomIT )
	{
		for(auto menuIT = (*roomIT)->menus.begin() ; menuIT != (*roomIT)->menus.end() ; ++ menuIT )
		{
			delete *menuIT;
		}
		delete *roomIT;
	}
}
void WebMenu::Create(MainWindow* poolMainWindow , const std::string&	thisroom)
{
	this->PoolMainWindow = poolMainWindow;
	this->thisroom = thisroom;
	auto webmenu = this->PoolMainWindow->Config.FindGetsToMap("webmenu__");
	std::map<std::string,Menu*> alreadykey;

	for(auto it = webmenu.begin() ; it != webmenu.end() ; ++it)
	{
		auto key = it->first.c_str();
		int sep = XLStringUtil::strpos(key,"_");
		if (sep <= 0)
		{
			continue;
		}

		std::string commonkey = std::string(key,key + sep);			//aircon_name -> aircon
		std::string optionkey = key + sep + 1;		//aircon_name -> name

		//機能として予約されているものは飛ばす.
		if (optionkey == "name" || optionkey == "icon" || optionkey == "state")
		{
			continue;
		}
		std::string menuname = webmenu[commonkey + "_name"];
		std::string menuimage = webmenu[commonkey + "_icon"];
		std::string menustate = webmenu[commonkey + "_state"];
		std::string actionname = it->second;
		std::string action = key;

		AddMenu(this->thisroom,"",menuname,menuimage,menustate,actionname,action);
	}
}

void WebMenu::AddMenu(const std::string& roomname,const std::string& roomip,const std::string& menuname,const std::string& menuimage,const std::string& menustate,const std::string& actionname,const std::string& actioncommand)
{
	Room * room;
	auto roomIT = this->rooms.begin();
	for( ; roomIT != this->rooms.end() ; ++roomIT ) if ((*roomIT)->name == roomname) break;
	if ( roomIT == rooms.end() )
	{
		room = new Room;
		room->ip =  roomip;
		room->name = roomname;
		if (roomname == this->thisroom)
		{//自分の管轄であれば、 ipをカラにします。
			room->ip = "";
		}
		this->rooms.push_back(room );
	}
	else
	{
		room = *roomIT;
	}


	Menu* menu;

	auto menuIT = room->menus.begin();
	for( ; menuIT != room->menus.end() ; ++menuIT ) if ((*menuIT)->name == menuname) break;
	if ( menuIT == room->menus.end() )
	{
		menu = new Menu;
		menu->name = menuname;
		room->menus.push_back(menu);
	}
	else
	{
		menu = *menuIT;
	}
	menu->image = menuimage;
	menu->status = menustate;


	Action* action;
	auto actionIT = menu->actions.begin();
	for( ; actionIT != menu->actions.end() ; ++actionIT ) if ((*actionIT)->name == actionname) break;
	if ( actionIT == menu->actions.end() )
	{
		action = new Action;
		action->name = actionname;
		menu->actions.push_back(action);
	}
	else
	{
		action = *actionIT;
	}
	action->action = actioncommand;
}

const std::vector<WebMenu::Room*>* WebMenu::getRooms() const
{
	return &this->rooms;
}
const std::string WebMenu::getRoomName() const
{
	return this->thisroom;
}
bool WebMenu::checkRoomname(const std::string& roomname) const
{
	auto roomIT = this->rooms.begin();
	for( ; roomIT != this->rooms.end() ; ++roomIT ) if ((*roomIT)->name == roomname) break;
	return ( roomIT != rooms.end() );
}

xreturn::r<bool> WebMenu::Fire(const CallbackDataStruct* callback,const std::string& roomname,const std::string& actionname)
{
	if ( actionname.find("action__") != 0)
	{
		return xreturn::error("アクション " + actionname + " の名前が正しくありません。 アクションは、 action__hogehoge のように、 action__ というprefixが必要です。");
	}
	std::string stripactionanme = actionname.c_str() + sizeof("action__") - 1; //action__aircon_max -> aircon_max

	Room * room;
	auto roomIT = this->rooms.begin();
	for( ; roomIT != this->rooms.end() ; ++roomIT ) if ((*roomIT)->name == roomname) break;
	if ( roomIT == rooms.end() )
	{
		return false;
	}
	else
	{
		room = *roomIT;
	}


	for(auto menuIT = room->menus.begin(); menuIT != room->menus.end() ; ++menuIT )
	{
		for(auto actionIT = (*menuIT)->actions.begin(); actionIT != (*menuIT)->actions.end() ; ++actionIT )
		{
			if ( (*actionIT)->action == stripactionanme )
			{
				return this->Fire(callback,roomname,(*menuIT)->name,(*actionIT)->name);
			}
		}
	}
	return false;
}

xreturn::r<bool> WebMenu::Fire(const CallbackDataStruct* callback,const std::string& roomname,const std::string& menuname,const std::string& actionname)
{
	Room * room;
	auto roomIT = this->rooms.begin();
	for( ; roomIT != this->rooms.end() ; ++roomIT ) if ((*roomIT)->name == roomname) break;
	if ( roomIT == rooms.end() )
	{
		return false;
	}
	else
	{
		room = *roomIT;
	}


	Menu * menu;
	auto menuIT = room->menus.begin();
	for( ; menuIT != room->menus.end() ; ++menuIT ) if ((*menuIT)->name == menuname) break;
	if ( menuIT == room->menus.end() )
	{
		return false;
	}
	else
	{
		menu = *menuIT;
	}


	Action* action;
	auto actionIT = menu->actions.begin();
	for( ; actionIT != menu->actions.end() ; ++actionIT ) if ((*actionIT)->name == actionname) break;
	if ( actionIT == menu->actions.end() )
	{
		return false;
	}
	action = *actionIT;

	//コマンド実行
	if (room->ip.empty())
	{
		//コールバックを実行
		this->PoolMainWindow->ActionScriptManager.Regist(callback,"action__" + action->action);
	}
	else
	{//他のノード
	}

	//ステータスを更新
	menu->status = action->name;

	return true;
}

