#pragma once

//atl使えない人のための、超単機能の CComPtr
template<typename _INTERFACE>class RSimpleComPtr
{
public:
	RSimpleComPtr()
	{
		this->Object = NULL;
	}
	virtual ~RSimpleComPtr()
	{
		if (this->Object)
		{
			Object->Release();
			this->Object = NULL;
		}
	}

	//構築
	HRESULT CoCreateInstance(REFCLSID clsid,unsigned long type = CLSCTX_ALL)
	{
		assert(this->Object == NULL);
		return ::CoCreateInstance(clsid, NULL, type, __uuidof(_INTERFACE) , (void**)&this->Object);
	}
	//構築 by 文字列
	HRESULT CoCreateInstance(wchar_t* progid,unsigned long type = CLSCTX_ALL)
	{
		assert(this->Object == NULL);

		CLSID clsid;
		HRESULT hr;
		hr = CLSIDFromProgID(progid, &clsid);
		if (hr != S_OK) return hr;

		return ::CoCreateInstance(clsid, NULL, type, __uuidof(clsid) , (void**)&this->Object);
	}

	//ラップしているオブジェクトを参照
	_INTERFACE* operator->() const
	{
		assert(this->Object != NULL);
		return this->Object;
	}
	//オブジェクトの書き換え
	_INTERFACE** operator&() 
	{
		assert(this->Object == NULL);	//あるとダメってことで。
		return &this->Object;
	}
	//インターフェースを取り出す場合
	operator _INTERFACE*() const
	{
		return this->Object;
	}

	//QueryInterfaceという名のキャスト.
	template<typename _CASTINTERFACE> HRESULT QueryInterface(_CASTINTERFACE** target)
	{
		assert(this->Object != NULL);
		return this->Object->QueryInterface(__uuidof(_CASTINTERFACE),(void**)target );
	}
	

private:
	_INTERFACE* Object;
};
