#include "common.h"
#include "Config.h"
#include "ScriptManager.h"
#include "resource.h"

#include "MainWindow.h"
const std::string HIDDEN_WINDOW_NAME = "hidden";
const int WM_THREAD_CALLBACK		 = WM_USER + 1;
const int WM_USER_TASKTRAY_CLICK	 = WM_USER + 2;
const int WM_TASKBAR_CREATED		 = WM_USER + 3;


xreturn::r<bool> MainWindow::Create(bool isdebug,HINSTANCE instance,const std::string & classname)
{
	this->MainThreadID = ::GetCurrentThreadId();
	this->WindowInstance = instance;

	WNDCLASS myProg;
	myProg.style            =CS_HREDRAW | CS_VREDRAW;
	myProg.lpfnWndProc      =methodcallback::registstdcall<struct _mainwindow_winproc,WNDPROC>(this, &MainWindow::WndProc);
	myProg.cbClsExtra       =0;
	myProg.cbWndExtra       =0;
	myProg.hInstance        =this->WindowInstance ;
	myProg.hIcon            =NULL;
	myProg.hCursor          =LoadCursor(NULL, IDC_ARROW);
	myProg.hbrBackground    = (HBRUSH)GetStockObject(WHITE_BRUSH);
	myProg.lpszMenuName     =NULL;
	myProg.lpszClassName    = classname.c_str();
	if (!RegisterClass(&myProg))
	{
		return xreturn::error("windowクラスの登録に失敗!");
	}

	this->MainWindowHandle = CreateWindow(classname.c_str()
		,HIDDEN_WINDOW_NAME.c_str()
		,WS_OVERLAPPEDWINDOW
		,CW_USEDEFAULT
		,CW_USEDEFAULT
		,400
		,400
		,NULL
		,NULL
		,this->WindowInstance 
		,NULL);
	if (isdebug)
	{
		this->LogLevel = LOG_LEVEL_DEBUG;
		OpenLoggerWindow();
		ShowWindow(this->MainWindowHandle,SW_SHOW);
		UpdateWindow(this->MainWindowHandle);
	}
	else
	{
		this->LogLevel = LOG_LEVEL_DEBUG;
	}

	OnInit();
	return true;
}

//既に動いていないかチェックする.
//(static メソッド)
bool MainWindow::checkAlreadyRunning() 
{
	//mutexでもいいけど、ここではmessageを送信したいので
	//main windowがあるかどうかを検索する.
	HWND hwnd = ::GetTopWindow(::GetDesktopWindow());
	do
	{
		TCHAR title[ MAX_PATH ]= {0};
		::GetWindowText( hwnd, title, sizeof(title) );

		if ( lstrcmp(title , HIDDEN_WINDOW_NAME.c_str() ) != 0 )
		{
			continue;
		}
		//見つけた!
//		::PostMessage(hwnd , WM_COMMAND, (WPARAM)IDM_FRONT, 0); 
		//既に動いてます!
		return true;
	}
	while( (hwnd = ::GetNextWindow(hwnd,GW_HWNDNEXT)) !=NULL );

	//まだ動いていません!
	return false;
}


LRESULT MainWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//メッセージによる分岐
	switch (msg) {
		case WM_CREATE:
//			this->Queue.setWindowHandle(hWnd);
			if (! OnCreate() )
			{
				return -1;
			}
			break;
		case WM_DESTROY:
			OnDestory();
			break;
		case WM_THREAD_CALLBACK:
			if (wParam && lParam)
			{
				((_THREAD_CALLBACK)wParam)(lParam);
			}
			break;
		case WM_USER_TASKTRAY_CLICK:
			OnTaskTrayClick(wParam, lParam);
			break;
		case WM_TASKBAR_CREATED:
			OnTaskbarCreated(wParam, lParam);
			break;
		case WM_COMMAND:
			OnCommand(wParam, lParam);
			break;
		default:
			return(DefWindowProc(hWnd, msg, wParam, lParam));
	}
	return (0L);
}

void MainWindow::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if (wParam == ID_MENU_QUIT)
	{
		SendMessage(this->MainWindowHandle,WM_DESTROY,0,0);
	}
	else if (wParam == ID_MENU_DEBUG)
	{
		if ( this->IsOpenConsole )
		{
			CloseLoggerWindow();
		}
		else
		{
			OpenLoggerWindow();
		}
	}
	else if (wParam == ID_MENU_SETTING)
	{
	this->SyncInvokeLog("ID_MENU_SETTING",LOG_LEVEL_DEBUG);
	}
}

//終了時
void MainWindow::OnDestory()
{
	this->SyncInvokeLog("プログラムを終了させます。");
	//タスクトレイからアイコンを消す.
	DelNotifyIcon();
	//ウィンドウハンドルのクリア.
	this->MainWindowHandle = NULL;
	//自滅用のメッセージを送信.
	PostQuitMessage(0);
}

//作成された時
bool MainWindow::OnCreate()
{
	return true;
}

//通知領域に追加
bool MainWindow::AddNotifyIcon()
{
	//タスクトレイにアイコンを出す
	this->NotifyIcon.cbSize = sizeof(NOTIFYICONDATA);
	this->NotifyIcon.uID = 0;
	this->NotifyIcon.hWnd = this->MainWindowHandle;
	this->NotifyIcon.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	this->NotifyIcon.hIcon = LoadIcon(this->WindowInstance,MAKEINTRESOURCE(IDI_ICON1));
	this->NotifyIcon.uCallbackMessage = WM_USER_TASKTRAY_CLICK;
	lstrcpyA( this->NotifyIcon.szTip, "ないちちボイスコマンダー by rti\r\nコマンドー。繰り返します。コマンドー。" );

	// アイコンを登録できるかエラーになるまで繰り返す
	while (true)
	{
		if (Shell_NotifyIcon(NIM_ADD, &this->NotifyIcon))
		{
			// 登録できたら終わり
			break;
		}
		else
		{
			// タイムアウトかどうか調べる
			if (::GetLastError() != ERROR_TIMEOUT)
			{
				// アイコン登録エラー
				return false;
			}

			// 登録できていないことを確認する
			if (Shell_NotifyIcon(NIM_MODIFY, &this->NotifyIcon))
			{
				// 登録できていた
				break;
			}
			else
			{
				// 登録できていなかった
				::Sleep(1000);
			}
		}
	}
	return true;
}

//タスクトレイからアイコンを消す
bool MainWindow::DelNotifyIcon()
{
	::Shell_NotifyIcon( NIM_DELETE, &this->NotifyIcon );
	return true;
}

//タスクトレイのアイコンがクリックされたとき
LRESULT MainWindow::OnTaskTrayClick(WPARAM wParam, LPARAM lParam)
{
	// カーソルの現在位置を取得
	POINT point;
	::GetCursorPos(&point);

	switch(lParam) 
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		SetForegroundWindow(this->MainWindowHandle);
		{
			HMENU menu = LoadMenu( this->WindowInstance , MAKEINTRESOURCE(IDR_MENU1) );
			if (!menu)
			{
				break;
			}
			HMENU menu2 = GetSubMenu(menu,0);
			if (!menu2)
			{
				break;
			}
			TrackPopupMenu(menu2,TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y,0,this->MainWindowHandle,NULL);
			PostMessage(this->MainWindowHandle,WM_NULL,0,0);
		}
	}
	return 0;
}

//シェルが死んだ時に飛んでくるメッセージ
LRESULT MainWindow::OnTaskbarCreated(WPARAM wParam, LPARAM lParam)
{
	this->AddNotifyIcon();
	return 0;
}


//Popupメッセージを出す.
void MainWindow::PopupMessage(const std::string & title,const std::string & message,DWORD icon )
{
	//バルーンを出す.
	NOTIFYICONDATA	sData;
	ZeroMemory(&sData, sizeof(NOTIFYICONDATA));
	sData.cbSize		= sizeof(NOTIFYICONDATA);
	sData.uFlags		= NIF_INFO;
	sData.hWnd			= this->MainWindowHandle;
	sData.dwInfoFlags	= icon;
	sData.uTimeout		= 2000;

	lstrcpyA(sData.szInfoTitle,title.c_str() );
	lstrcpyA(sData.szInfo,  message.c_str() );

	Shell_NotifyIcon(NIM_MODIFY,&sData);
}


//初期化を行う所
bool MainWindow::OnInit()
{
	bool ret = this->AddNotifyIcon();
	if (!ret)
	{
		return FALSE;
	}

	this->SyncInvokeLog("naichichi2起動初期化シーケンススタート",LOG_LEVEL_DEBUG);

	//全体的の初期化を行います。

	//config読み込み
	this->Config.Create();

	//音声認識エンジン
	{
		//認識エンジンの初期化(lua初期より前にやらないとダメ)
		std::string engine = this->Config.Get("reco__engine", "" );
		std::list<std::string> yobikakeListArray = this->Config.FindGets("reco__yobikake__");
		double temporaryRuleConfidenceFilter = this->Config.GetDouble("reco__temporary_rule_confidence_filter",0);
		double yobikakeRuleConfidenceFilter = this->Config.GetDouble("reco__yobikake_rule_confidence_filter",0);
		double basicRuleConfidenceFilter = this->Config.GetDouble("reco__command_rule_confidence_filter",0);
		bool dictation_Filter = this->Config.GetBool("reco__dictation_filter",true);
	
		this->Recognition.Create(engine, this , yobikakeListArray ,temporaryRuleConfidenceFilter,yobikakeRuleConfidenceFilter, basicRuleConfidenceFilter,dictation_Filter);
	}

	//合成音声
	{
		std::string speakEngine = this->Config.Get("speak__engine","");
		std::string speakPerson = this->Config.Get("speak__person","");
		int         speakRate  = this->Config.GetInt("speak__rate",0);
		int         speakPitch  = this->Config.GetInt("speak__pitch",0);
		int         speakVolume  = this->Config.GetInt("speak__volume",0);
		this->Speak.Create(speakEngine,this,speakRate,speakPitch,speakVolume,speakPerson);
	}

	//モーション
	{
		//むりむり、間に合わない。
	}

	//Httpd
	{
		int port = this->Config.GetInt("httpd__port",15550);
		int threadcount = this->Config.GetInt("httpd__workerthread",2);
		std::string webroot = this->Config.Get("httpd__webroot", this->Config.GetBaseDirectory() + "\\webroot");
		std::map<std::string,std::string> allowext = this->Config.FindGetsToMap("httpd__allowext_"); 

		char myhostname[256] ;
		gethostname(myhostname, sizeof(myhostname));

		std::string accesstoken = this->Config.Get("httpd__accesstoken", myhostname ); 
		this->Httpd.Create(this,port,threadcount,webroot,accesstoken,allowext);
	}
	//トリガーマネージャ
	{
		this->TriggerManager.Create(this);
	}

	//メディアファイル
	{
		std::list<std::string> mediaDirectoryListArray = this->Config.FindGets("media__directory"); 
		std::map<std::string,std::string> mediaTargetext = this->Config.FindGetsToMap("media__targetext_"); 
		std::map<std::string,std::string> mediaDefualtIcon = this->Config.FindGetsToMap("media__defualticon_"); 
		std::string dbpath = this->Config.GetBaseDirectory() + "\\media.db"; //this->Config.Get("media__dbpath",""); 
		std::string filenamehelperLua = this->Config.Get("media__filenamehelper","config_media_filename.lua"); 
		this->Media.Create(this,mediaDirectoryListArray,dbpath,filenamehelperLua,mediaTargetext,mediaDefualtIcon);
	}

	//アクションのスクリプト管理
	{
		int threadcount2 = this->Config.GetInt("action__workerthread",2);
		this->ActionScriptManager.Create(this,threadcount2);
	}

	//シナリオスクリプト管理
	{
		this->ScriptManager.Create(this);
		this->SyncInvokeLog("naichichi2起動初期化シーケンス完了",LOG_LEVEL_DEBUG);
	}

	return true;
}

void MainWindow::SyncInvoke(std::function<void (void) > func)
{
	if (::GetCurrentThreadId() == this->MainThreadID) 
	{
		ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドですね
		//即実行
		func();
		return ;
	}

	struct callbackClass
	{
		callbackClass(std::function<void (void) > func) : func(func)
		{
			
		}
		static void callback(int p)
		{
			ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません

			callbackClass* _this = (callbackClass*)p;
			_this->func();
		}

		std::function<void (void) > func;
	};

	callbackClass call(func);
	SendMessage(
		 this->MainWindowHandle
		,WM_THREAD_CALLBACK
		,(WPARAM)callbackClass::callback
		,(LPARAM)&call
	);
}

void MainWindow::AnSyncInvoke(std::function<void (void) > func)
{
	struct callbackClass
	{
		callbackClass(std::function<void (void) > func) : func(func)
		{
			
		}
		static void callback(int p)
		{
			ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません

			callbackClass* _this = (callbackClass*)p;
			_this->func();

			delete _this;
		}

		std::function<void (void) > func;
	};
	callbackClass *call = new callbackClass(func);
	PostMessage(
		 this->MainWindowHandle
		,WM_THREAD_CALLBACK
		,(WPARAM)callbackClass::callback
		,(LPARAM)call
	);
}

void MainWindow::SyncInvokePopupMessage(const std::string& title,const std::string& message,DWORD icon)
{
	this->SyncInvoke( [=](){
		this->PopupMessage( title, message,icon) ;
	});
}
void MainWindow::SyncInvokeError(const std::string& message)
{
	SyncInvokeLog(message,LOG_LEVEL_ERROR);
	this->SyncInvoke( [=](){
		MessageBox( this->MainWindowHandle, message.c_str() , "error" ,MB_OK | MB_ICONERROR);
	});
}

void MainWindow::SyncInvokeScriptError(const std::string& message)
{
	SyncInvokeLog(message,LOG_LEVEL_FAIL_NOTIFY);
	this->SyncInvoke( [=](){
		MessageBox( this->MainWindowHandle, message.c_str() , "script error" ,MB_OK | MB_ICONERROR);
	});
}
void MainWindow::SyncInvokeWarning(const std::string& message)
{
	SyncInvokeLog(message,LOG_LEVEL_WARNING);
	this->SyncInvoke( [=](){
		MessageBox( this->MainWindowHandle, message.c_str() , "warning" ,MB_OK | MB_ICONWARNING);
	});
}

void MainWindow::SyncInvokeLog(const std::string& log,LOG_LEVEL level )
{
	if (!this->IsOpenConsole || this->LogLevel > level )
	{
		return ;
	}

	this->SyncInvoke( [=](){
			HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);

			if (level <= LOG_LEVEL_DEBUG)
			{
				WriteConsole(handle , "DEBUG      :" , 11, NULL, NULL);
			}
			else if (level <= LOG_LEVEL_NOTIFY)
			{
				SetConsoleTextAttribute(handle, FOREGROUND_GREEN);	//文字を強調
				WriteConsole(handle , "NOTIFY     :" , 11, NULL, NULL);
			}
			else if (level <= LOG_LEVEL_FAIL_NOTIFY)
			{
				SetConsoleTextAttribute(handle, FOREGROUND_RED | FOREGROUND_GREEN); //文字を黄色
				WriteConsole(handle , "FAIL NOTIFY:" , 11, NULL, NULL);
			}
			else if (level <= LOG_LEVEL_WARNING)
			{
				SetConsoleTextAttribute(handle, BACKGROUND_RED | BACKGROUND_BLUE); //背景を紫
				WriteConsole(handle , "WARNING    :" , 11, NULL, NULL);
			}
			else if (level <= LOG_LEVEL_ERROR)
			{
				SetConsoleTextAttribute(handle, BACKGROUND_RED); //背景を赤
				WriteConsole(handle , "ERROR      :" , 11, NULL, NULL);
			}
			//黒白に戻す.
			SetConsoleTextAttribute(handle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
			//本文
			WriteConsole(handle , log.c_str() , log.size(), NULL, NULL);
			WriteConsole(handle , "\r\n" , 2 , NULL, NULL);
		}
	);
}

//ログ用のコンソールを開きます。
xreturn::r<bool> MainWindow::OpenLoggerWindow()
{
	if ( this->IsOpenConsole )
	{
		return false;
	}

	//http://eternalwindows.jp/windevelop/console/console05.html
	//コンソールを作り、閉じるボタンを無効にする.
	if ( ! AllocConsole() )
	{
		DWORD lasterror = ::GetLastError();
		return xreturn::windowsError("コンソールを確保できません",lasterror);
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

	//閉じるボタンを消します。
	HMENU hmenu = GetSystemMenu(GetConsoleWindow(), FALSE);
	RemoveMenu(hmenu, SC_CLOSE, MF_BYCOMMAND);

	//開きました!
	this->IsOpenConsole = true;

	this->SyncInvokeLog("デバッグ出力画面です。\r\nこの画面を閉じるには、タスクトレイのポップアップからもう一度デバッグを選択してください。\r\n");
	return true;
}

//ログ用のコンソールを閉じます.
void MainWindow::CloseLoggerWindow()
{
	if (! this->IsOpenConsole )
	{
		return ;
	}
	FreeConsole();

	this->IsOpenConsole = false;
	return ;
}
