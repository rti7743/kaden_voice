#pragma once
#include <string>
#include <vector>

class WebMenu
{
public:
	struct Action
	{
		std::string name;
		std::string action;
	};
	struct Menu
	{
		std::string name;
		std::string image;
		std::string status;

		std::vector<Action*>	actions;
	};
	struct Room
	{
		std::string name;
		std::string ip;
		std::vector<Menu*>	menus;
	};
private:
	std::string			thisroom;
	std::vector<Room*>	rooms;
	MainWindow* PoolMainWindow;
public:
	WebMenu();
	virtual ~WebMenu();
	void Create(MainWindow* poolMainWindow , const std::string&	thisroom);
	const std::vector<Room*>* getRooms() const;
	const std::string getRoomName() const;
	bool checkRoomname(const std::string& roomname) const;
	void AddMenu(const std::string& roomname,const std::string& roomip,const std::string& menuname,const std::string& menuimage,const std::string& menustate,const std::string& actionname,const std::string& actioncommand);
	xreturn::r<bool> Fire(const CallbackDataStruct* callback,const std::string& roomname,const std::string& action);
	xreturn::r<bool> Fire(const CallbackDataStruct* callback,const std::string& roomname,const std::string& menuname,const std::string& actionname);
};
