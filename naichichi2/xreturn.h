#pragma once

#include <string>
#include <typeinfo>
//
//戻り値にエラーを絡ませられるようにしてみたライブラリ
//
//

#ifdef _MSC_VER
#pragma once
#include <windows.h>
#include <stdlib.h>
#include <dbghelp.h>
#elif __GNUC__
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <cxxabi.h>		//デマングル
						//入っていないなら yum install binutils-devel とかしてね!
						// #include <bfd.h> 自体をコメントアウトすると機能全体をOFFにできるよ!!
#endif


namespace xreturn
{


//xreturn::error として使ってね。
class error : public std::exception {
protected:
	std::string message; //
	int code;			 //std::exceptionがなぜか保持してくれないので自分で保持する.

	void*	StackBuffer[50]; //スタックトレース
	int     StackBufferSize; //実際入っている容量 50以下.
public:

	error() : StackBufferSize(0) , code(0)
	{
	}
	error(const std::string & message , int code)
		: std::exception() , message(message) , code(code)
	{
		recordTrace();
	}
	error(const std::string & message)
		: std::exception() , message(message) , code(0)
	{
		recordTrace();
	}
	error(const error& error)
		: std::exception() 
	{
		this->message = error.message;
		this->code = error.code;

		memcpy(this->StackBuffer ,error.StackBuffer , sizeof(this->StackBuffer) );
		this->StackBufferSize = error.StackBufferSize;
	}
	error(const error& error,const std::string & message)
		: std::exception()
	{
		this->message = message + error.message;
		this->code = error.code;

		recordTrace();
	}
	error(const error& error,const std::string & message,int code)
		: std::exception()
	{
		this->message = message + error.message;
		this->code = code;

		recordTrace();
	}
	virtual ~error() throw()
	{
	}

	void recordTrace()
	{
	}

	virtual int getErrorCode() const
	{
		return this->code;
	}
	virtual std::string getErrorMessage() const
	{
		return this->message;
	}
	virtual std::string getFullErrorMessage() const
	{
		char stacktraceBuffer[1024] ={0};

		std::stringstream out;
		out << "===message===" << std::endl;
		out << this->message << std::endl << std::endl;
		out << "===code===" << std::endl;
		out << this->code << std::endl << std::endl;
		out << "===stacktrace===" << std::endl;
		out << stacktraceBuffer << std::endl << std::endl;

		return out.str();
	}
};


//エラーをラップする構造
template <typename TYPE>
class r
	{
private:
	TYPE value;
	xreturn::error	error;
	bool            iserror;
	mutable bool    nigirichubushi; //すまぬ・・・ const auto に対応するために、 mutableにさせてくれ。
public:
	r()
	{
		this->iserror = false;
		this->nigirichubushi = false;
	}
	virtual ~r()
	{
		if (this->nigirichubushi)
		{
			//非常に悩ましいが、あえてデストラクタで例外を投げる。
			//エラーをチェックしないで握りつぶすようなコードで例外処理を行うにはこれしか方法がない。。。
			if ( this->isError() )
			{
				//deleteで消し終わってから、throw exception したいんだけど、 投げる例外が消したいメモリ空間にあるわけで・・・
				//これはあもりにもジャンクコードすぎるのでなんとかしたいんだけど。
				throw this->error;
			}
		}
	}

//boolにキャストする場合
//これで実装すると、 xreturn::r<bool> の時に衝突するから operator で逃げる.
//	operator bool() 
//	{
//		this->nigirichubushi = false;
//		return this->isOK();
//	}
	bool operator !() const
	{
		return this->isError();
	}
 	bool operator ==(bool t) const
	{
		if ( typeid(TYPE) == typeid(bool) )
		{//xreturn::r<bool>の場合は判別不能なので普通にキャスト operator TYPE() に飛ばす.
			return (*this);
		}
		return (t == true && this->isOK()) || (t == false && this->isOK());
	}

	operator TYPE() const
	{
		this->nigirichubushi = false;
		if ( this->isError() )
		{
			throw this->error; //ここで例外を投げる
		}
		return this->value;
	}
	r<TYPE>& operator =(const TYPE & a)
	{
		this->value = a;
		this->iserror = false;
		this->nigirichubushi = false;
		a.nigirichubushi = false;

		return *this;
	}
	r(const TYPE& a)
	{
		this->value = a;
		this->iserror = false;
		this->nigirichubushi = false;
	}
	r(const xreturn::error& e)
	{
		this->error = e;
		this->iserror = true;
		this->nigirichubushi = true;
	}

	bool isOK()  const
	{
		this->nigirichubushi = false;   //すまぬ・・・ const auto に対応するために、 mutableにさせてくれ。
		return this->iserror == false;
	}
	bool isError()  const
	{
		this->nigirichubushi = false; 
		return this->iserror == true;
	}
	TYPE check() const
	{
		this->nigirichubushi = false; 
		return (*this);
	}
	int getErrorCode() const
	{
		return this->error.getErrorCode();
	}
	std::string getErrorMessage() const
	{
		return this->error.getErrorMessage();
	}
	std::string getFullErrorMessage() const
	{
		return this->error.getFullErrorMessage();
	}
	const xreturn::error& getError() const
	{
		return this->error;
	}

	//例外を投げる.
	void throwException() const
	{
		this->nigirichubushi = false; 
		throw this->error;
	}


	//現状の型情報を取得する
	const std::type_info& type() const
	{
		//成功している場合は、保持している型をそのまま
		if ( this->isOK() )
		{
			return typeid(TYPE);
		}
		//エラーの場合は、エラー型をいただく.
		return this->error.type();
	}
};



//おまけ
#ifdef _WIN32
//windowsエラーメッセージを xreturn の中に組み込む感じのやつ
class windowsError : public xreturn::error
{
public:
	windowsError()
	{
		DWORD lasterror = ::GetLastError();
		this->code = (int)lasterror;
		this->appendMessage(lasterror);
	}
	windowsError(DWORD lasterror)
	{
		this->code = (int)lasterror;
		this->appendMessage(lasterror);
	}
	windowsError(const std::string & message,DWORD lasterror) 
	{
		this->code = (int)lasterror;
		this->message = message;

		this->appendMessage(lasterror);
	}
	windowsError(const std::string & message) 
	{
		this->code = (int)::GetLastError();
		this->message = message;

		this->appendMessage(this->code);
	}
	virtual ~windowsError()
	{
	}
protected:
	void appendMessage(DWORD lasterror)
	{
		void* msgBuf;
		::FormatMessage( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			lasterror,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // デフォルト言語
			(LPTSTR) &msgBuf,
			0,
			NULL 
		);
		if (msgBuf == NULL)
		{
			std::stringstream out;
			out << "can not find error. error code:" << lasterror;
			this->message += out.str();
			return ;
		}

		this->message = this->message + " " + (const char*)msgBuf;
		::LocalFree( msgBuf );
	}
};

#endif //_WIN32

#ifdef errno
//errno を xreturn 風に
class errnoError : public xreturn::error
{
public:
	errnoError()
	{
		int lasterror = errno;
		this->code = (int)lasterror;
		this->appendMessage(lasterror);
	}
	errnoError(int lasterror)
	{
		this->code = lasterror;
		this->appendMessage(lasterror);
	}
	errnoError(const std::string & message,int lasterror) 
	{
		this->code = lasterror;
		this->message = message;

		this->appendMessage(lasterror);
	}
	errnoError(const std::string & message) 
	{
		this->code = errno;
		this->message = message;

		this->appendMessage(this->code);
	}
protected:
	void appendMessage(int lasterror)
	{
#ifdef __STDC_WANT_SECURE_LIB__
		char buffer[1024];
		strerror_s( buffer , sizeof(buffer)-1, lasterror);
		buffer[sizeof(buffer)-1] = '\0';

		this->message = this->message + " " + buffer;
#else
		this->message = this->message + " " + strerror( lasterror );
#endif
	}
};
#endif //errno

}; //namespace xreturn

