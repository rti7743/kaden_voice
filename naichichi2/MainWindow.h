#pragma once

#include "common.h"
#include "Config.h"
#include "ScriptManager.h"
#include "Recognition_Factory.h"
#include "Speak_Factory.h"
#include "HttpServer.h"
#include "ActionScriptManager.h"
#include "MediaFileIndex.h"
#include "TriggerManager.h"
#include "WebMenu.h"
#include "MecabControl.h"
#include <gdiplus.h>

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

	xreturn::r<bool> Create(bool isdebug,HINSTANCE instance,const std::string & classname="mainwindow");

	int BlockMessageLoop()
	{
		MSG msg;
		while (GetMessage(&msg, NULL, 0, 0)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		return (msg.wParam);
	}

	static bool MainWindow::checkAlreadyRunning() ;
	
	void SyncInvokePopupMessage(const std::string& title,const std::string& message,DWORD icon = NIIF_INFO);
	//システムに影響を与えるエラー
	void SyncInvokeError(const std::string& message);
	//luaスクリプトなどで発生したエラー
	void SyncInvokeScriptError(const std::string& message);
	//ワーニング
	void SyncInvokeWarning(const std::string& message);
	//ログを出力させる.
	void MainWindow::SyncInvokeLog(const std::string& log,LOG_LEVEL level = LOG_LEVEL_NOTIFY);

	//初期化を行う所
	typedef void (*_THREAD_CALLBACK)(int p);
	//メインスレッドコールバック 関数オブジェクト
	void MainWindow::SyncInvoke(std::function<void (void) > func);
	//メインスレッドで後で実行するコールバック
	void MainWindow::AsyncInvoke(std::function<void (void) > func);

	
	
	
private:
	virtual LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	bool MainWindow::OnInit();

	//終了時
	void OnDestory();
	//作成された時
	bool MainWindow::OnCreate();
	//通知領域に追加
	bool MainWindow::AddNotifyIcon();
	//タスクトレイからアイコンを消す
	bool MainWindow::DelNotifyIcon();
	//タスクトレイのアイコンがクリックされたとき
	LRESULT MainWindow::OnTaskTrayClick(WPARAM wParam, LPARAM lParam);
	//シェルが死んだ時に飛んでくるメッセージ
	LRESULT MainWindow::OnTaskbarCreated(WPARAM wParam, LPARAM lParam);
	//Popupメッセージを出す.
	void MainWindow::PopupMessage(const std::string & title,const std::string & message,DWORD icon = NIIF_INFO);
	//ログ用のコンソールを開きます。
	xreturn::r<bool> MainWindow::OpenLoggerWindow();
	//ログ用のコンソールを閉じます.
	void MainWindow::CloseLoggerWindow();
	void MainWindow::OnCommand(WPARAM wParam, LPARAM lParam);

	HINSTANCE WindowInstance;
	HWND MainWindowHandle;
	NOTIFYICONDATA NotifyIcon;
public:
	Config					Config;
	ScriptManager			ScriptManager;
	Recognition_Factory		Recognition;
	Speak_Factory			Speak;
	HttpServer				Httpd;
	ActionScriptManager		ActionScriptManager;
	MediaFileIndex			Media;
	TriggerManager			TriggerManager;
	WebMenu					WebMenu;

	MecabControl			Mecab;

	DWORD MainThreadID;
	bool IsOpenConsole;
	LOG_LEVEL LogLevel;
};
	

class COMInit
{
public:
	COMInit()
	{
		::CoInitialize(NULL);
	}
	virtual ~COMInit()
	{
		::CoUninitialize();
	}
};

class WinSockInit
{
public:
	WinSockInit()
	{
//		WSADATA wsaData;
//		::WSAStartup(2 , &wsaData);
	}
	virtual ~WinSockInit()
	{
//		::WSACleanup();
	}
};

class GdiPlusInit
{
public:
	GdiPlusInit()
	{
		Gdiplus::GdiplusStartup(&gdiToken, &gdiSI, NULL);
	}
	virtual ~GdiPlusInit()
	{
		Gdiplus::GdiplusShutdown(gdiToken);
	}
private:
	Gdiplus::GdiplusStartupInput gdiSI;
	ULONG_PTR gdiToken;
};
