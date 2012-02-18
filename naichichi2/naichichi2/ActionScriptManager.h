// ActionScriptManager.h: ActionScriptManager クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ActionScriptManager_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_)
#define AFX_ActionScriptManager_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
class ActionScriptManager
{
public:
	ActionScriptManager();
	virtual ~ActionScriptManager();
	
	xreturn::r<bool> ActionScriptManager::Create(MainWindow* poolMainWindow,int threadcount);
	xreturn::r<bool> ActionScriptManager::Regist(const CallbackDataStruct& callback , const std::string & actionName );

private:
	xreturn::r<bool> ActionScriptManager::ThreadRun(const CallbackDataStruct& callback ,const std::list<std::string>& args );

	boost::thread_group ThreadGroup;
	boost::asio::io_service IOService;
	boost::asio::io_service::work Work;

	MainWindow* PoolMainWindow;
};



#endif // !defined(AFX_ActionScriptManager_H__1477FE93_D7A8_4F29_A369_60E33C71B2B7__INCLUDED_)
