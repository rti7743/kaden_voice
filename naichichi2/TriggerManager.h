// TriggerManager.h: TriggerManager クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TriggerManager_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_)
#define AFX_TriggerManager_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
class TriggerManager
{
public:
	TriggerManager();
	virtual ~TriggerManager();
	
	xreturn::r<bool> Create(MainWindow* poolMainWindow);
	xreturn::r<bool> Regist(const CallbackDataStruct* callback , const std::string & menuName, const std::string & triggername );
	xreturn::r<std::string> Call( const std::string & triggername ,const std::map<std::string,std::string>& args);
	bool IsExist(const std::string & triggerName) const;
	std::map<std::string,std::string> GetAllMenuname() const;
private:
	struct triggerStruct
	{
		std::string menuname;
		const CallbackDataStruct* callback;

		triggerStruct(const std::string& menuname,const CallbackDataStruct* callback) : menuname(menuname) , callback(callback)
		{
		};
	};
	std::map<std::string,const triggerStruct*> TriggerMap;
	MainWindow* PoolMainWindow;
};



#endif // !defined(AFX_TriggerManager_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_)
