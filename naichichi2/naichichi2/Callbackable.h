#pragma once

class CallbackDataStruct;

class Callbackable
{
public:
	Callbackable();
	virtual ~Callbackable();

	virtual std::string callbackFunction(const CallbackDataStruct* callback,const std::map<std::string , std::string> & match) = 0;

	//コールバックが不要になった時に呼ばれる 自分から this-> で読んではいけない。
	virtual void unrefCallback(const CallbackDataStruct* callback);
	//新しいコールバックを定義する
	virtual CallbackDataStruct* CreateCallback(int _func,int _func2 = 0);
protected:
	//コールバック一覧

	std::list<CallbackDataStruct*> callbackHistoryList;
};

const int NO_CALLBACK = INT_MAX;

//コールバックするデータを保持する
//コピーされまくるので軽量の構造にするべし.
class CallbackDataStruct
{
public:
	CallbackDataStruct(Callbackable* _runner,int _func,int _func2) : runner(_runner) , func(_func) , func2(func2)
	{
#if _DEBUG
		threadid = ::boost::this_thread::get_id();
#endif //_DEBUG
	}
	int getFunc() const
	{
#if _DEBUG
		//違うスレッドで実行してはいけない!!
		assert(this->threadid == ::boost::this_thread::get_id());
#endif //_DEBUG
		return this->func;
	}
	int getFunc2() const
	{
#if _DEBUG
		//違うスレッドで実行してはいけない!!
		assert(this->threadid == ::boost::this_thread::get_id());
#endif //_DEBUG
		return this->func2;
	}
	Callbackable* getRunner() const
	{
#if _DEBUG
		//違うスレッドで実行してはいけない!!
		assert(this->threadid == ::boost::this_thread::get_id());
#endif //_DEBUG
		return this->runner;
	}

private:
	Callbackable* runner;
	int func;
	int func2;
#if _DEBUG
	::boost::thread::id threadid;
#endif //_DEBUG
};
