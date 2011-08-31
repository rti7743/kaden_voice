
// naichichiDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "naichichi.h"
#include "naichichiDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//タスクトレイがクリックされたとき
const int WM_USER_TASKTRAY_CLICK = WM_USER+100;
const int WM_USER_SPEECH_RECOGNITIONED = WM_USER+101;
const int WM_TASKBAR_CREATED = RegisterWindowMessage(_T("TaskbarCreated"));

// CnaichichiDlg ダイアログ
CnaichichiDlg::CnaichichiDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CnaichichiDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CnaichichiDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CnaichichiDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_USER_TASKTRAY_CLICK, &CnaichichiDlg::OnTaskTrayClick)
	ON_MESSAGE(WM_USER_SPEECH_RECOGNITIONED, &CnaichichiDlg::OnSpeechRecognitioned)
	ON_REGISTERED_MESSAGE(WM_TASKBAR_CREATED, OnTaskbarCreated)
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_ACTIVATE()
	ON_COMMAND(IDM_QUIT, OnMenuCommandExit)
END_MESSAGE_MAP()


//既に動いていないかチェックする.
//(static メソッド)
bool CnaichichiDlg::checkAlreadyRunning() 
{
	//mutexでもいいけど、ここではmessageを送信したいので
	//main windowがあるかどうかを検索する.
	HWND hwnd = ::GetTopWindow(::GetDesktopWindow());
	do
	{
		TCHAR title[ MAX_PATH ]= {0};
		::GetWindowText( hwnd, title, sizeof(title) );

		if ( lstrcmp(title , "Niaichichi_Hidden_window" ) != 0 )
		{
			continue;
		}
		//見つけた!
		::PostMessage(hwnd , WM_COMMAND, (WPARAM)IDM_FRONT, 0); 
		//既に動いてます!
		return true;
	}
	while( (hwnd = ::GetNextWindow(hwnd,GW_HWNDNEXT)) !=NULL );

	//まだ動いていません!
	return false;
}

// CnaichichiDlg メッセージ ハンドラー

BOOL CnaichichiDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// "バージョン情報..." メニューをシステム メニューに追加します。

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	// TODO: 初期化をここに追加します。
	ModifyStyleEx(WS_EX_APPWINDOW, 0);
	ShowWindow( SW_HIDE );

	//どうしてもダイアログを消せなかったので、、これで逃げてみる。
	MoveWindow(1,1,1,1,TRUE);

	//通知領域にアイコンなどを追加.
	bool ret = this->AddNotifyIcon();
	if (!ret)
	{
		return FALSE;
	}

	//音声認識エンジンスタート!
	this->SpeechRecognition.Create("","_vicecommand.xml",this->m_hWnd ,WM_USER_SPEECH_RECOGNITIONED );

	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}


// ダイアログに最小化ボタンを追加する場合、アイコンを描画するための
//  下のコードが必要です。ドキュメント/ビュー モデルを使う MFC アプリケーションの場合、
//  これは、Framework によって自動的に設定されます。

void CnaichichiDlg::OnPaint()
{
	//非表示
	ModifyStyleEx(WS_EX_APPWINDOW, 0);
	ShowWindow( SW_HIDE );

	if (IsIconic())
	{
		CPaintDC dc(this); // 描画のデバイス コンテキスト

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// クライアントの四角形領域内の中央
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// アイコンの描画
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ユーザーが最小化したウィンドウをドラッグしているときに表示するカーソルを取得するために、
//  システムがこの関数を呼び出します。
HCURSOR CnaichichiDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CnaichichiDlg::PostNcDestroy()
{
	// TODO: ここに特定なコードを追加するか、もしくは基本クラスを呼び出してください。

	//アイコンを消す.
	this->DelNotifyIcon();

	CDialogEx::PostNcDestroy();
}




//通知領域に追加
bool CnaichichiDlg::AddNotifyIcon()
{
	//タスクトレイにアイコンを出す
	this->NotifyIcon.cbSize = sizeof(NOTIFYICONDATA);
	this->NotifyIcon.uID = 0;
	this->NotifyIcon.hWnd = m_hWnd;
	this->NotifyIcon.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	this->NotifyIcon.hIcon = AfxGetApp()->LoadIcon( IDR_MAINFRAME );
	this->NotifyIcon.uCallbackMessage = WM_USER_TASKTRAY_CLICK;
	lstrcpy( this->NotifyIcon.szTip, "ないちちボイスコマンダー by rti\r\nコマンドー。繰り返します。コマンドー。" );

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
bool CnaichichiDlg::DelNotifyIcon()
{
	::Shell_NotifyIcon( NIM_DELETE, &this->NotifyIcon );
	return true;
}

//タスクトレイのアイコンがクリックされたとき
afx_msg LRESULT CnaichichiDlg::OnTaskTrayClick(WPARAM wParam, LPARAM lParam)
{
	// カーソルの現在位置を取得
	POINT point;
	::GetCursorPos(&point);

	switch(lParam) 
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		SetForegroundWindow();	
		CMenu menu;
		VERIFY(menu.LoadMenu(IDR_MENU1));
		CMenu* pPopup = menu.GetSubMenu(0);
		ASSERT(pPopup != NULL);
		pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
		PostMessage(WM_NULL);
		break;
	}
	return 0;
}

//シェルが死んだ時に飛んでくるメッセージ
afx_msg LRESULT CnaichichiDlg::OnTaskbarCreated(WPARAM wParam, LPARAM lParam)
{
	this->AddNotifyIcon();
	return 0;
}



//終了
void CnaichichiDlg::OnMenuCommandExit() 
{
	// 終了が選ばれたらメインウィンドウを閉じてプログラムを終了する。
	PostMessage(WM_CLOSE);
}


void CnaichichiDlg::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	lpwndpos->flags &= ~SWP_SHOWWINDOW;
	CDialogEx::OnWindowPosChanged(lpwndpos);

	// TODO: ここにメッセージ ハンドラー コードを追加します。
}

//音声認識されたとき
afx_msg LRESULT CnaichichiDlg::OnSpeechRecognitioned(WPARAM wParam, LPARAM lParam)
{
	//結果の取得
	if ( ! this->SpeechRecognition.Listen() )
	{
		return 0;
	}

	//結果を見ていく.
	std::string text = this->SpeechRecognition.getResultString();
	std::string mtext = this->SpeechRecognition.getResultMap("NAME");

	//コマンドを認識できたらしい.
	if (mtext != "")
	{
		std::string title = "音声認識「" + text + "」 します。";
		std::string message = "「" + mtext + "」を実行します。";

		//バルーンを出す.
		NOTIFYICONDATA	sData;
		ZeroMemory(&sData, sizeof(NOTIFYICONDATA));
		sData.cbSize		= sizeof(NOTIFYICONDATA);
		sData.uFlags		= NIF_INFO;
		sData.hWnd			= this->m_hWnd;
		sData.dwInfoFlags	= NIIF_INFO;
		sData.uTimeout		= 3000;

		lstrcpy(sData.szInfoTitle,title.c_str() );
		lstrcpy(sData.szInfo,  message.c_str() );

		Shell_NotifyIcon(NIM_MODIFY,&sData);


		//コマンドの実行
		::ShellExecute(NULL,NULL,mtext.c_str(),NULL,NULL,0);

	}

	return 0;
}

