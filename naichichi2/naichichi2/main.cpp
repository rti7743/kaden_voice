#include "common.h"
#include "MainWindow.h"


#if _MSC_VER
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst,
                   LPSTR lpszCmdLine, int nCmdShow)
#else
int main()
#endif
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません
//	_CrtSetBreakAlloc(24862);

	int ret = -1;
	{
		COMInit cominit;
		WinSockInit winsockinit;

		MainWindow win;
		try
		{
//			SexyTestRunner::allrun();
puts("win.Create");
			win.Create(true);
puts("win.BlockMessageLoop");
			ret = win.BlockMessageLoop();
		}
		catch(xreturn::error & e)
		{
			win.SyncInvokeError( e.getFullErrorMessage() );
		}
	}
	
#if _MSC_VER && _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	return ret;
}

