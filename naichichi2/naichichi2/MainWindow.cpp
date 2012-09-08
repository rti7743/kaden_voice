#include "common.h"
#include "Config.h"
#include "ScriptManager.h"
#include "XLStringUtil.h"

#include "MainWindow.h"

#if _MSC_VER
	#include "resource.h"
	#include "methodcallback.h"

	const char* HIDDEN_WINDOW_NAME = "hidden";
#endif	//_MSC_VER
const int WM_THREAD_CALLBACK		 = WM_USER + 1;
const int WM_USER_TASKTRAY_CLICK	 = WM_USER + 2;
const int WM_TASKBAR_CREATED		 = WM_USER + 3;

bool MainWindow::Create(bool isdebug)
{
	this->MainThreadID = ::boost::this_thread::get_id();
	this->StopFlag = false;

#if _MSC_VER
	this->WindowInstance = GetModuleHandle(0);
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
	myProg.lpszClassName    = HIDDEN_WINDOW_NAME;
	if (!RegisterClass(&myProg))
	{
		throw XLException("windowクラスの登録に失敗!");
	}

	this->MainWindowHandle = CreateWindow(HIDDEN_WINDOW_NAME
		,HIDDEN_WINDOW_NAME
		,WS_OVERLAPPEDWINDOW
		,CW_USEDEFAULT
		,CW_USEDEFAULT
		,400
		,400
		,NULL
		,NULL
		,this->WindowInstance 
		,NULL);
	ShowWindow(this->MainWindowHandle,SW_SHOW);
	UpdateWindow(this->MainWindowHandle);
#else
	this->MainWindowHandle = NULL;
	this->WindowInstance = NULL;
#endif

	if (isdebug)
	{
		this->LogLevel = LOG_LEVEL_DEBUG;
		OpenLoggerWindow();
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
#if _MSC_VER
	//mutexでもいいけど、ここではmessageを送信したいので
	//main windowがあるかどうかを検索する.
	HWND hwnd = ::GetTopWindow(::GetDesktopWindow());
	do
	{
		TCHAR title[ MAX_PATH ]= {0};
		::GetWindowText( hwnd, title, sizeof(title) );

		if ( lstrcmp(title , HIDDEN_WINDOW_NAME ) != 0 )
		{
			continue;
		}
		//見つけた!
//		::PostMessage(hwnd , WM_COMMAND, (WPARAM)IDM_FRONT, 0); 
		//既に動いてます!
		return true;
	}
	while( (hwnd = ::GetNextWindow(hwnd,GW_HWNDNEXT)) !=NULL );
#else
#endif
	//まだ動いていません!
	return false;
}

int MainWindow::BlockMessageLoop()
{
#if _MSC_VER
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (msg.wParam);
#else
	while(!this->StopFlag)
	{
		{
			boost::unique_lock<boost::mutex> al(this->Lock);
			if (this->Queue.size() <= 0)
			{
				this->queue_wait.wait(al);
			}
		}
		//寝起きかもしれないので終了条件の確認.
		if (this->StopFlag)
		{
			return 0;
		}
		//メッセージをすべて再生
		RunMessagePump();
	}
	return 0;
#endif
}
LRESULT MainWindow::WndProc(HWND hWnd, unsigned int msg, WPARAM wParam, LPARAM lParam)
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
#if _MSC_VER
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
#endif
	}
	return (0L);
}

void MainWindow::OnCommand(WPARAM wParam, LPARAM lParam)
{
#if _MSC_VER
	if (wParam == ID_MENU_QUIT)
	{//終了
		SendMessage(this->MainWindowHandle,WM_DESTROY,0,0);
	}
	else if (wParam == ID_MENU_DEBUG)
	{//デバッグメニューを開く
		if ( this->IsOpenConsole )
		{
			CloseLoggerWindow();
		}
		else
		{
			OpenLoggerWindow();
		}
	}
	else if (wParam == ID_MENU_WEB)
	{//web画面を開く
		std::string weburl = this->Httpd.getWebURL("/remocon/");
		ShellExecute(NULL,NULL,weburl.c_str() , NULL,NULL,0);
	}
#endif
}


//windows依存部
#if _MSC_VER
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
#else
//linux依存部

//メッセージを処理する
void MainWindow::RunMessagePump()
{
	boost::unique_lock<boost::mutex> al(this->Lock);
	for(auto it = this->Queue.begin() ; it != this->Queue.end() ; ++it )
	{
		LRESULT r = WndProc(0, it->message,it->wParam,it->lParam);
		if (it->result && it->updatewait)
		{
			*(it->result) = r;
			*(it->updatewait) = true;
		}
	}
	this->Queue.clear();
}
LRESULT MainWindow::SendMessage(HWND dummyWindowHandle,unsigned int message,WPARAM wParam,LPARAM lParam)
{
	int result = 0;
	bool updatewait = false;
	struct Message m = {0};
	m.message = message;
	m.wParam = wParam;
	m.lParam = lParam;
	m.result = &result;
	m.updatewait = &updatewait;
	
	//仕事を積む
	{
		boost::unique_lock<boost::mutex> al(this->Lock);
		this->Queue.push_back(m);
	}
	//スレッドwakeup
	{
		this->queue_wait.notify_all();
	}

	if (this->MainThreadID == ::boost::this_thread::get_id())
	{
		//メインスレッドであれば、メッセージポンプを回す。 そうじゃないと詰まってしまう。
		RunMessagePump();
	}
	
	//値を取得する(値を取得するまでブロック).
	while(!updatewait)
	{
		sleep(0);
	}

	return result;
}
void MainWindow::PostMessage(HWND dummyWindowHandle,unsigned int message,WPARAM wParam,LPARAM lParam)
{
	struct Message m = {0};
	m.message = message;
	m.wParam = wParam;
	m.lParam = lParam;
	m.result = NULL;
	m.updatewait = NULL;

	//仕事を積む
	{
		boost::unique_lock<boost::mutex> al(this->Lock);
		this->Queue.push_back(m);
	}
	//スレッドwakeup
	{
		this->queue_wait.notify_all();
	}
}


#endif

//終了時
void MainWindow::OnDestory()
{
	this->WriteLog("プログラムを終了させます。");
	//停止フラグを立てる.
	this->StopFlag = true;

#if _MSC_VER
	//今あるメッセージ・キューを空にする.
	{
		MSG msg;
		while(PeekMessage (&msg,NULL,0,0,PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	this->WriteLog("メッセージループ完了。");
#endif

	//config を保存する.
	this->Config.overrideSave();
	this->WriteLog("configセーブ完了。");

#if _MSC_VER
	//タスクトレイからアイコンを消す.
	DelNotifyIcon();
	//自滅用のメッセージを送信.
	PostQuitMessage(0);
#endif
	//ウィンドウハンドルのクリア.
	this->MainWindowHandle = NULL;
}

//作成された時
bool MainWindow::OnCreate()
{
	return true;
}



//Popupメッセージを出す.
void MainWindow::PopupMessage(const std::string & title,const std::string & message )
{
#if _MSC_VER
	//バルーンを出す.
	NOTIFYICONDATA	sData;
	ZeroMemory(&sData, sizeof(NOTIFYICONDATA));
	sData.cbSize		= sizeof(NOTIFYICONDATA);
	sData.uFlags		= NIF_INFO;
	sData.hWnd			= this->MainWindowHandle;
	sData.dwInfoFlags	= NIIF_INFO;
	sData.uTimeout		= 2000;

	lstrcpyA(sData.szInfoTitle,title.c_str() );
	lstrcpyA(sData.szInfo,  message.c_str() );

	Shell_NotifyIcon(NIM_MODIFY,&sData);
#endif
}

//初期化を行う所
bool MainWindow::OnInit()
{
#if _MSC_VER
	bool ret = this->AddNotifyIcon();
	if (!ret)
	{
		return false;
	}
#endif

	this->SyncInvokeLog("naichichi2起動初期化シーケンススタート",LOG_LEVEL_DEBUG);

	//全体的の初期化を行います。

	//config読み込み
	this->Config.Create();

	//部屋の家電をwebから操作するときのインターフェース
	{
		std::string thisroom = this->Config.Get("room","myroom"); 
		this->WebMenu.Create(this,thisroom);
	}
	//音声認識エンジン
	{
		//認識エンジンの初期化(lua初期より前にやらないとダメ)
		std::string engine = this->Config.Get("reco__engine", "" );
		std::list<std::string> yobikakeListArray = this->Config.FindGets("reco__yobikake__");
		std::list<std::string> cancelListArray = this->Config.FindGets("reco__cancel__");
		double temporaryRuleConfidenceFilter = this->Config.GetDouble("reco__temporary_rule_confidence_filter",0.7);

#if _MSC_VER
		std::string reco__logdir = this->Config.Get("reco__logdir", "c:\\TEMP" );
#else
		std::string reco__logdir = this->Config.Get("reco__logdir", "/tmp" );
#endif
		this->Recognition.Create(engine, this , yobikakeListArray ,cancelListArray,temporaryRuleConfidenceFilter,reco__logdir);
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

	//Httpd
	{
		int port = this->Config.GetInt("httpd__port",15550);
		int threadcount = this->Config.GetInt("httpd__workerthread",2);
		std::string webroot = this->Config.Get("httpd__webroot", this->Config.GetBaseDirectory() + "\\webroot");
		std::map<std::string,std::string> allowext = this->Config.FindGetsToMap("httpd__allowext_",true); 

		this->Httpd.Create(this,port,threadcount,webroot,allowext);
	}
	//トリガーマネージャ
	{
		this->TriggerManager.Create(this);
	}

	//アクションのスクリプト管理
	{
		this->ActionScriptManager.Create(this);
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
	//停止フラグが有効な場合は無視する。
	if (this->StopFlag)
	{
		return ;
	}

	if (::boost::this_thread::get_id() == this->MainThreadID) 
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

void MainWindow::AsyncInvoke(std::function<void (void) > func)
{
	//停止フラグが有効な場合は無視する。
	if (this->StopFlag)
	{
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

void MainWindow::SyncInvokePopupMessage(const std::string& title,const std::string& message)
{
	//停止フラグが有効な場合は無視する。
	if (this->StopFlag)
	{
		return ;
	}

	this->SyncInvoke( [=](){
		this->PopupMessage( title, message) ;
	});
}
void MainWindow::SyncInvokeError(const std::string& message)
{
	//停止フラグが有効な場合は無視する。
	if (this->StopFlag)
	{
		return ;
	}

	SyncInvokeLog(message,LOG_LEVEL_ERROR);
#if _MSC_VER
	//ポップアップを出す
	this->SyncInvoke( [=](){
		MessageBox( this->MainWindowHandle, message.c_str() , "error" ,MB_OK | MB_ICONERROR);
	});
#else
#endif
}

void MainWindow::SyncInvokeScriptError(const std::string& message)
{
	//停止フラグが有効な場合は無視する。
	if (this->StopFlag)
	{
		return ;
	}

	SyncInvokeLog(message,LOG_LEVEL_FAIL_NOTIFY);
#if _MSC_VER
	//ポップアップを出す
	this->SyncInvoke( [=](){
		MessageBox( this->MainWindowHandle, message.c_str() , "script error" ,MB_OK | MB_ICONERROR);
	});
#else
#endif
}
void MainWindow::SyncInvokeWarning(const std::string& message)
{
	//停止フラグが有効な場合は無視する。
	if (this->StopFlag)
	{
		return ;
	}

	SyncInvokeLog(message,LOG_LEVEL_WARNING);
#if _MSC_VER
	//ポップアップを出す
	this->SyncInvoke( [=](){
		MessageBox( this->MainWindowHandle, message.c_str() , "warning" ,MB_OK | MB_ICONWARNING);
	});
#else
#endif
}

void MainWindow::WriteLog(const std::string& log,LOG_LEVEL level )
{
	if (!this->IsOpenConsole || this->LogLevel > level )
	{
		return ;
	}

#if _MSC_VER
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
#endif

	if (level <= LOG_LEVEL_DEBUG)
	{
#if _MSC_VER
		WriteConsole(handle , "DEBUG      :" , 11, NULL, NULL);
#else
		puts("DEBUG      :");
#endif
	}
	else if (level <= LOG_LEVEL_NOTIFY)
	{
#if _MSC_VER
		SetConsoleTextAttribute(handle, FOREGROUND_GREEN);	//文字を強調
		WriteConsole(handle , "NOTIFY     :" , 11, NULL, NULL);
#else
		puts("NOTIFY     :");
#endif
	}
	else if (level <= LOG_LEVEL_FAIL_NOTIFY)
	{
#if _MSC_VER
		SetConsoleTextAttribute(handle, FOREGROUND_RED | FOREGROUND_GREEN); //文字を黄色
		WriteConsole(handle , "FAIL NOTIFY:" , 11, NULL, NULL);
#else
		puts("FAIL NOTIFY:");
#endif
	}
	else if (level <= LOG_LEVEL_WARNING)
	{
#if _MSC_VER
		SetConsoleTextAttribute(handle, BACKGROUND_RED | BACKGROUND_BLUE); //背景を紫
		WriteConsole(handle , "WARNING    :" , 11, NULL, NULL);
#else
		puts("WARNING    :");
#endif
	}
	else if (level <= LOG_LEVEL_ERROR)
	{
#if _MSC_VER
		SetConsoleTextAttribute(handle, BACKGROUND_RED); //背景を赤
		WriteConsole(handle , "ERROR      :" , 11, NULL, NULL);
#else
		puts("ERROR      :");
#endif
	}
#if _MSC_VER
	//黒白に戻す.
	SetConsoleTextAttribute(handle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	//本文 長すぎる文章を一気に書くと表示されないので、一行づつくぎって表示する.
	std::list<std::string> l = XLStringUtil::split("\r\n",log);
	for(std::list<std::string>::iterator it = l.begin() ; it != l.end() ; ++it)
	{
		WriteConsole(handle , it->c_str() , it->size(), NULL, NULL);
		WriteConsole(handle , "\r\n" , 2 , NULL, NULL);
	}
#else
	puts(log.c_str());
#endif
}

void MainWindow::SyncInvokeLog(const std::string& log,LOG_LEVEL level )
{
	if (!this->IsOpenConsole || this->LogLevel > level )
	{
		return ;
	}

	//停止フラグが有効な場合は無視する。
	if (this->StopFlag)
	{
		return ;
	}

	this->SyncInvoke( [=](){
			WriteLog(log,level);
		}
	);
}

//ログ用のコンソールを開きます。
bool MainWindow::OpenLoggerWindow()
{
#if _MSC_VER
	if ( this->IsOpenConsole )
	{
		return false;
	}

	//http://eternalwindows.jp/windevelop/console/console05.html
	//コンソールを作り、閉じるボタンを無効にする.
	if ( ! AllocConsole() )
	{
		DWORD lasterror = ::GetLastError();
		throw XLException(XLException::StringWindows() + "コンソールを確保できません",lasterror);
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

	//閉じるボタンを消します。
	HMENU hmenu = GetSystemMenu(GetConsoleWindow(), FALSE);
	RemoveMenu(hmenu, SC_CLOSE, MF_BYCOMMAND);

	//開きました!
	this->IsOpenConsole = true;
	this->SyncInvokeLog("デバッグ出力画面です。\r\nこの画面を閉じるには、タスクトレイのポップアップからもう一度デバッグを選択してください。\r\n");
#else
#endif
	return true;
}

//ログ用のコンソールを閉じます.
void MainWindow::CloseLoggerWindow()
{
#if _MSC_VER
	if (! this->IsOpenConsole )
	{
		return ;
	}
	FreeConsole();
#else
#endif
	this->IsOpenConsole = false;
	return ;
}

//config の中のパスを作る
std::string MainWindow::GetConfigBasePath(const std::string& path) const
{
	return XLStringUtil::pathcombine( this->Config.GetBaseDirectory(), path);
}

