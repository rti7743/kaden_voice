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
void WebMenu::AddMenu(const std::string& menuname,const std::string& image)
{
	AddMenu(this->thisroom ,"",menuname , image );
}
void WebMenu::AddMenu(const std::string& roomname,const std::string& roomip,const std::string& menuname,const std::string& image)
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
	menu->image = image;
}
void WebMenu::AddMenuSub(const std::string& menuname,const std::string& actionname,const std::string& image,const CallbackDataStruct* callback)
{
	AddMenuSub(this->thisroom ,menuname , actionname, image, callback);
}
bool WebMenu::AddMenuSub(const std::string& roomname,const std::string& menuname,const std::string& actionname,const std::string& image,const CallbackDataStruct* callback)
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
		action = new Action;
		action->name = actionname;
		menu->actions.push_back(action);
	}
	else
	{
		action = *actionIT;
	}
	action->image = image;
	action->func = callback;
	return true;
}
bool WebMenu::Fire(const std::string& roomname,const std::string& menuname,const std::string& actionname)
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
		this->PoolMainWindow->ScriptManager.WebMenuCall(action->func);
	}
	else
	{//他のノード
	}

	//ステータスを更新
	menu->status = menu->name;

	return true;
}

