#include "common.h"
#include "MainWindow.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst,
                   LPSTR lpszCmdLine, int nCmdShow)
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません
//	_CrtSetBreakAlloc(24862);

	int ret = -1;
	{
		COMInit cominit;
		WinSockInit winsockinit;
		GdiPlusInit gdiPlusInit;

		MainWindow win;
		try
		{
			SexyTestRunner::allrun();

			win.Create(true,hInstance);
			ret = win.BlockMessageLoop();
		}
		catch(xreturn::error & e)
		{
			win.SyncInvokeError( e.getFullErrorMessage() );
		}
	}
	
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	return ret;
}

