#pragma once

#include "common.h"
#include "Config.h"
#include "ScriptManager.h"
#include "Recognition_Factory.h"
#include "Speak_Factory.h"
#include "HttpServer.h"
#include "ActionScriptManager.h"
#include "TriggerManager.h"
#include "WebMenu.h"

enum LOG_LEVEL
{
	 LOG_LEVEL_DEBUG = 0			//デバッグなどに必要な情報。
	,LOG_LEVEL_NOTIFY = 3			//通知
	,LOG_LEVEL_FAIL_NOTIFY = 4		//失敗した通知
	,LOG_LEVEL_WARNING = 7			//警告
	,LOG_LEVEL_ERROR = 9			//エラー
};

class MainWindow
{
public:
	MainWindow()
	{
		this->WindowInstance = NULL;
		this->MainWindowHandle = NULL;
		this->IsOpenConsole = false;
	}
	virtual ~MainWindow()
	{
	}

	xreturn::r<bool> Create(bool isdebug);

	int BlockMessageLoop();

	static bool checkAlreadyRunning() ;
	
	void SyncInvokePopupMessage(const std::string& title,const std::string& message);
	//システムに影響を与えるエラー
	void SyncInvokeError(const std::string& message);
	//luaスクリプトなどで発生したエラー
	void SyncInvokeScriptError(const std::string& message);
	//ワーニング
	void SyncInvokeWarning(const std::string& message);
	//ログを出力させる.
	void SyncInvokeLog(const std::string& log,LOG_LEVEL level = LOG_LEVEL_NOTIFY);

	//初期化を行う所
	typedef void (*_THREAD_CALLBACK)(int p);
	//メインスレッドコールバック 関数オブジェクト
	void SyncInvoke(std::function<void (void) > func);
	//メインスレッドで後で実行するコールバック
	void AsyncInvoke(std::function<void (void) > func);

	
	
	
private:
	virtual LRESULT WndProc(HWND hWnd, unsigned int msg, WPARAM wParam, LPARAM lParam);
	bool OnInit();

	//終了時
	void OnDestory();
	//作成された時
	bool OnCreate();
	//Popupメッセージを出す.
	void PopupMessage(const std::string & title,const std::string & message);
	//ログ用のコンソールを開きます。
	xreturn::r<bool> OpenLoggerWindow();
	//ログ用のコンソールを閉じます.
	void CloseLoggerWindow();
	void OnCommand(WPARAM wParam, LPARAM lParam);

#if _MSC_VER
//windows依存部
	//通知領域に追加
	bool AddNotifyIcon();
	//タスクトレイからアイコンを消す
	bool DelNotifyIcon();
	//タスクトレイのアイコンがクリックされたとき
	LRESULT OnTaskTrayClick(WPARAM wParam, LPARAM lParam);
	//シェルが死んだ時に飛んでくるメッセージ
	LRESULT OnTaskbarCreated(WPARAM wParam, LPARAM lParam);

#else
//linux依存部
	//windowsの SendMessage相当のことをする
	LRESULT SendMessage(HWND dummyWindowHandle,unsigned int message,WPARAM wParam,LPARAM lParam);
	//windowsの PostMessage相当のことをする
	void PostMessage(HWND dummyWindowHandle,unsigned int message,WPARAM wParam,LPARAM lParam);
	//メッセージを処理する
	void RunMessagePump();
#endif

public:
	class Config					Config;
	class ScriptManager				ScriptManager;
	class Recognition_Factory		Recognition;
	class Speak_Factory				Speak;
	class HttpServer				Httpd;
	class ActionScriptManager		ActionScriptManager;
	class TriggerManager			TriggerManager;
	class WebMenu					WebMenu;

private:
	::boost::thread::id MainThreadID;
	bool IsOpenConsole;
	LOG_LEVEL LogLevel;
	bool StopFlag;
	HINSTANCE WindowInstance;
	HWND MainWindowHandle;

#if _MSC_VER
//windows依存部
	NOTIFYICONDATA NotifyIcon;
#else
//linuxでもwindowsみたいなメッセージループを擬似的に作る
	boost::mutex   Lock;
	boost::condition_variable queue_wait;
	struct Message
	{
		unsigned int	message;
		WPARAM	wParam;
		LPARAM	lParam;
		int*	result;
		bool*	updatewait;
	};
	std::list<Message> Queue;
#endif
};

class COMInit
{
public:
	COMInit()
	{
#if _MSC_VER
		::CoInitialize(NULL);
#else
#endif
	}
	virtual ~COMInit()
	{
#if _MSC_VER
		::CoUninitialize();
#else
#endif
	}
};

class WinSockInit
{
public:
	WinSockInit()
	{
#if _MSC_VER
//		WSADATA wsaData;
//		::WSAStartup(2 , &wsaData);
#else
#endif
	}
	virtual ~WinSockInit()
	{
#if _MSC_VER
//		::WSACleanup();
#else
#endif
	}
};

