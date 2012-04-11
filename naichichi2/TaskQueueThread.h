#pragma once

class TaskQueueThread
{
public:
	TaskQueueThread()
	{
		this->IsDown = false;
		this->Thread = new boost::thread(boost::bind(&TaskQueueThread::Run,this));
	}
	virtual ~TaskQueueThread()
	{
		this->Kill();
		delete this->Thread;
	}

	template <typename Fn>
	void ASyncInvoke(Fn f) 
	{
		this->Serv.post(f);
	}
	template <typename Fn>
	auto SyncInvoke(Fn f) -> decltype(f()) 
	{
		typedef decltype(f()) RetT;
		boost::packaged_task<RetT> pt(f);
		boost::unique_future<RetT> uf(pt.get_future());
		this->Serv.post([&pt](){ return pt(); });
		return uf.get();
	}

	virtual void Kill() 
	{
		if (!this->IsDown)
		{
//			this->Thread->interrupt();
			this->Serv.stop();
			this->Thread->join();
			this->IsDown = true;
		}
	}
	virtual void Run()
	{
		boost::asio::io_service::work w(this->Serv);
		this->Serv.run();
	}

protected:

	bool IsDown;
	boost::asio::io_service Serv;
	boost::thread* Thread;
};

//メインスレッドに対するタスクキュー
//SendMessage(WM_USER + 123 ) とかもうやめよう!!
class MainTaskQueueThread
{
public:
	MainTaskQueueThread() 
	{
		this->IsDown = false;
	}
	virtual ~MainTaskQueueThread()
	{
		this->Kill();
	}

#ifdef _WINDOWS
	void PostWakeupMessage(UINT message)
	{
		//このプロセスがし有しているすべてのwindowに対して目覚めのメッセージを送る.
//これが発動しない・・・。
//		::PostThreadMessage(this->ThreadID , message , 0 , 0  );
		DWORD currentProcessID = ::GetCurrentProcessId();
		HWND hwnd = GetTopWindow(NULL);
		do {
			if(GetWindowLong( hwnd, GWL_HWNDPARENT) != 0 || !IsWindowVisible( hwnd))
			{
				continue;
			}
			DWORD processID;
			GetWindowThreadProcessId( hwnd, &processID);
			if(currentProcessID == processID)
			{
				::PostMessageA(hwnd , message , 0 , 0 );
			}
		} while((hwnd = GetNextWindow( hwnd, GW_HWNDNEXT)) != NULL);
	}
#endif

	template <typename Fn>
	void ASyncInvoke(Fn f) 
	{
		this->Serv.post(f);
#ifdef _WINDOWS
		//GetMessageしているかもしれないので嘘のメッセージでポンプを回す.
		PostWakeupMessage(WM_NULL);
#endif
	}
	template <typename Fn>
	auto SyncInvoke(Fn f) -> decltype(f()) 
	{
		typedef decltype(f()) RetT;
		boost::packaged_task<RetT> pt(f);
		boost::unique_future<RetT> uf(pt.get_future());
		this->Serv.post([&pt](){ return pt(); });
#ifdef _WINDOWS
		//GetMessageしているかもしれないので嘘のメッセージでポンプを回す.
		PostWakeupMessage(WM_NULL);
#endif
		return uf.get();
	}

	virtual void Kill() 
	{
		if (!this->IsDown)
		{
//			this->Thread->interrupt();
			this->Serv.stop();
			this->IsDown = true;
		}
	}
	
	virtual void Run(unsigned int message)
	{
		if (WM_NULL == message)
		{
			this->Serv.poll();
		}
	}

private:

	bool IsDown;
	boost::asio::io_service Serv;
};
