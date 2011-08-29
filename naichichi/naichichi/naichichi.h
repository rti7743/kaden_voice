
// naichichi.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです。
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h"		// メイン シンボル


// CnaichichiApp:
// このクラスの実装については、naichichi.cpp を参照してください。
//

class CnaichichiApp : public CWinApp
{
public:
	CnaichichiApp();

// オーバーライド
public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()
};

extern CnaichichiApp theApp;