// NiseMainDialog.cpp : 実装ファイル
//

#include "stdafx.h"
#include "naichichi.h"
#include "NiseMainDialog.h"
#include "afxdialogex.h"

// CNiseMainDialog ダイアログ

IMPLEMENT_DYNAMIC(CNiseMainDialog, CDialogEx)

CNiseMainDialog::CNiseMainDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(CNiseMainDialog::IDD, pParent)
{

}

CNiseMainDialog::~CNiseMainDialog()
{
}

void CNiseMainDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CNiseMainDialog, CDialogEx)
END_MESSAGE_MAP()


// CNiseMainDialog メッセージ ハンドラー


BOOL CNiseMainDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  ここに初期化を追加してください

	//ダイアログを最前面に
	SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}


BOOL CNiseMainDialog::DestroyWindow()
{
	// TODO: ここに特定なコードを追加するか、もしくは基本クラスを呼び出してください。

	return CDialogEx::DestroyWindow();
}

