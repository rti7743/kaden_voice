
// naichichiDlg.h : ヘッダー ファイル
//

#pragma once
#include "RSpeechRecognition.h"


// CnaichichiDlg ダイアログ
class CnaichichiDlg : public CDialogEx
{
// コンストラクション
public:
	CnaichichiDlg(CWnd* pParent = NULL);	// 標準コンストラクター

// ダイアログ データ
	enum { IDD = IDD_NAICHICHI_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV サポート

	public:
	//既に動いていないかチェックする.
	//(static メソッド)
	static bool CnaichichiDlg::checkAlreadyRunning() ;

// 実装
protected:
	HICON m_hIcon;
	//通知
	NOTIFYICONDATA NotifyIcon ;
	//音声認識
	RSpeechRecognition SpeechRecognition;

	//通知領域に追加
	bool CnaichichiDlg::AddNotifyIcon();
	//タスクトレイからアイコンを消す
	bool CnaichichiDlg::DelNotifyIcon();
	//タスクトレイがクリックされた場合
	afx_msg LRESULT CnaichichiDlg::OnTaskTrayClick(WPARAM wParam, LPARAM lParam);
	//音声認識されたとき
	afx_msg LRESULT CnaichichiDlg::OnSpeechRecognitioned(WPARAM wParam, LPARAM lParam);
	//シェルが死んだ時に飛んでくるメッセージ
	afx_msg LRESULT CnaichichiDlg::OnTaskbarCreated(WPARAM wParam, LPARAM lParam);
	//終了
	void CnaichichiDlg::OnMenuCommandExit() ;
	BOOL CnaichichiDlg::Create();

	// 生成された、メッセージ割り当て関数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	virtual void PostNcDestroy();
public:
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	afx_msg void OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2);
};
