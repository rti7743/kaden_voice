#include "common.h"
#include "Callbackable.h"

Callbackable::Callbackable()
{
}

Callbackable::~Callbackable()
{
	//メモリ解放
	CDeleteAll(this->callbackHistoryList);
}


//コールバックが不要になった時に呼ばれる 自分から this-> で読んではいけない。
void Callbackable::unrefCallback(const CallbackDataStruct* callback)
{
	for ( auto it = this->callbackHistoryList.begin(); it != this->callbackHistoryList.end() ; ++it)
	{
		if (*it == callback)
		{
			delete *it;
			this->callbackHistoryList.erase(it);
			break;
		}
	}
}

//新しいコールバックを定義する
CallbackDataStruct* Callbackable::CreateCallback(int _func,int _func2)
{
	CallbackDataStruct* callback = new CallbackDataStruct(this,_func ,_func2);
	this->callbackHistoryList.push_back(callback);

	return callback;
}
