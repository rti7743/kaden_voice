#include <iostream>

//関数オブジェクトやメソッドを関数ポインタに変換する。
//c_function.hppを元に自分でも作ってみた。
//違い  1.関数オブジェクトだけではなく、メソッドも渡せる
//      2.boost不要 :p)
//      3.stdcall にも対応できる。
//
//動作確認 VS2010

//メソッドに対するコールバック
namespace methodcallback
{
	//標準呼び出し
	namespace detail
	{
		template<typename _GUID,typename FUNC_FUNC,typename _CLASS,typename _METHOD>
		struct callcdecl
		{
#ifdef _MSC_VER
			static __declspec( thread )  _CLASS*  _class;
			static __declspec( thread )  _METHOD _method;
#elif __CXX0X_MODE__
			static thread_local _CLASS*  _class;
			static thread_local _METHOD _method;
#else
			//threadに対して使えない
			static _CLASS*  _class;
			static _METHOD _method;
#endif
			template<typename _RET,typename _P1> static _RET callback(_P1 p1){	return (_class->*_method)(p1);	}
			template<typename _RET,typename _P1,typename _P2> static _RET callback(_P1 p1,_P2 p2){	return (_class->*_method)(p1,p2);	}
			template<typename _RET,typename _P1,typename _P2,typename _P3> static _RET callback(_P1 p1,_P2 p2,_P3 p3){	return (_class->*_method)(p1,p2,p3);	}
			template<typename _RET,typename _P1,typename _P2,typename _P3,typename _P4> static _RET callback(_P1 p1,_P2 p2,_P3 p3,_P4 p4){	return (_class->*_method)(p1,p2,p3,p4);	}
			template<typename _RET,typename _P1,typename _P2,typename _P3,typename _P4,typename _P5> static _RET callback(_P1 p1,_P2 p2,_P3 p3,_P4 p4,_P5 p5){	return (_class->*_method)(p1,p2,p3,p4,p5);	}

			template<typename _P1> static void callback(_P1 p1){	(_class->*_method)(p1);	}
			template<typename _P1,typename _P2> static void callback(_P1 p1,_P2 p2){	(_class->*_method)(p1,p2);	}
			template<typename _P1,typename _P2,typename _P3> static void callback(_P1 p1,_P2 p2,_P3 p3){	(_class->*_method)(p1,p2,p3);	}
			template<typename _P1,typename _P2,typename _P3,typename _P4> static void callback(_P1 p1,_P2 p2,_P3 p3,_P4 p4){	(_class->*_method)(p1,p2,p3,p4);	}
			template<typename _P1,typename _P2,typename _P3,typename _P4,typename _P5> static void callback(_P1 p1,_P2 p2,_P3 p3,_P4 p4,_P5 p5){	(_class->*_method)(p1,p2,p3,p4,p5);	}
		};
		template <typename _GUID,typename FUNC_FUNC,typename _CLASS,typename _METHOD> _CLASS* callcdecl<_GUID,FUNC_FUNC,_CLASS,_METHOD>::_class = NULL;
		template <typename _GUID,typename FUNC_FUNC,typename _CLASS,typename _METHOD> _METHOD callcdecl<_GUID,FUNC_FUNC,_CLASS,_METHOD>::_method = NULL;
	}
	//コールバック登録
	template<typename _GUID,typename FUNC_FUNC,typename _CLASS,typename _METHOD>
	FUNC_FUNC regist(_CLASS* _class,_METHOD _method)
	{
		typedef detail::callcdecl<_GUID,FUNC_FUNC,_CLASS,_METHOD> DETAIL;
		DETAIL::_class = _class;
		DETAIL::_method = _method;
		return (FUNC_FUNC)(DETAIL::callback);
	}
	//METHODを省略した場合は operator() が呼ばれる.
	template<typename _GUID,typename FUNC_FUNC,typename _CLASS>
	FUNC_FUNC regist(_CLASS* _class)
	{
		return regist<_GUID,FUNC_FUNC>(_class,&_CLASS::operator());
	}

	//stdcall
	namespace detail
	{
		template<typename _GUID,typename FUNC_FUNC,typename _CLASS,typename _METHOD>
		struct callstdcall
		{
#ifdef _MSC_VER
			static __declspec( thread )  _CLASS*  _class;
			static __declspec( thread )  _METHOD _method;
#elif __CXX0X_MODE__
			static thread_local _CLASS*  _class;
			static thread_local _METHOD _method;
#else
			//threadに対して使えない
			static _CLASS*  _class;
			static _METHOD _method;
#endif
			template<typename _RET,typename _P1> static _RET __stdcall callback(_P1 p1){	return (_class->*_method)(p1);	}
			template<typename _RET,typename _P1,typename _P2> static _RET __stdcall callback(_P1 p1,_P2 p2){	return (_class->*_method)(p1,p2);	}
			template<typename _RET,typename _P1,typename _P2,typename _P3> static _RET __stdcall callback(_P1 p1,_P2 p2,_P3 p3){	return (_class->*_method)(p1,p2,p3);	}
			template<typename _RET,typename _P1,typename _P2,typename _P3,typename _P4> static _RET __stdcall callback(_P1 p1,_P2 p2,_P3 p3,_P4 p4){	return (_class->*_method)(p1,p2,p3,p4);	}
			template<typename _RET,typename _P1,typename _P2,typename _P3,typename _P4,typename _P5> static _RET __stdcall callback(_P1 p1,_P2 p2,_P3 p3,_P4 p4,_P5 p5){	return (_class->*_method)(p1,p2,p3,p4,p5);	}

			template<typename _P1> static void __stdcall callback(_P1 p1){ (_class->*_method)(p1);	}
			template<typename _P1,typename _P2> static void __stdcall callback(_P1 p1,_P2 p2){	(_class->*_method)(p1,p2);	}
			template<typename _P1,typename _P2,typename _P3> static void __stdcall callback(_P1 p1,_P2 p2,_P3 p3){	(_class->*_method)(p1,p2,p3);	}
			template<typename _P1,typename _P2,typename _P3,typename _P4> static void __stdcall callback(_P1 p1,_P2 p2,_P3 p3,_P4 p4){	(_class->*_method)(p1,p2,p3,p4);	}
			template<typename _P1,typename _P2,typename _P3,typename _P4,typename _P5> static void __stdcall callback(_P1 p1,_P2 p2,_P3 p3,_P4 p4,_P5 p5){	(_class->*_method)(p1,p2,p3,p4,p5);	}
		};
		template <typename _GUID,typename FUNC_FUNC,typename _CLASS,typename _METHOD> _CLASS* callstdcall<_GUID,FUNC_FUNC,_CLASS,_METHOD>::_class = NULL;
		template <typename _GUID,typename FUNC_FUNC,typename _CLASS,typename _METHOD> _METHOD callstdcall<_GUID,FUNC_FUNC,_CLASS,_METHOD>::_method = NULL;
	}
	//コールバック登録
	template<typename _GUID,typename FUNC_FUNC,typename _CLASS,typename _METHOD>
	FUNC_FUNC registstdcall(_CLASS* _class,_METHOD _method)
	{
		typedef detail::callstdcall<_GUID,FUNC_FUNC,_CLASS,_METHOD> DETAIL;
		DETAIL::_class = _class;
		DETAIL::_method = _method;
		return (FUNC_FUNC)(DETAIL::callback);
	}
	//METHODを省略した場合は operator() が呼ばれる.
	template<typename _GUID,typename FUNC_FUNC,typename _CLASS>
	FUNC_FUNC registstdcall(_CLASS* _class)
	{
		return registstdcall<_GUID,FUNC_FUNC>(_class,&_CLASS::operator());
	}
};
/*
struct AAA
{
	int Method(int ccc)
	{
		return 12 + ccc;
	}
};

struct BBB
{
	int operator() (int ccc)
	{
		return 12 + ccc;
	}
};

int main()
{
	int ans;

	//クラスメソッド AAA:Method
	AAA aaa;
	FUNC f = methodcallback::regist<struct a123,FUNC>(&aaa , &AAA::Method);
	ans = f(22);
	std::cout << "12+22=" << ans << std::endl;

	//関数オブジェクト operator() 
	BBB bbb;
	FUNC f2 = methodcallback::regist<struct a123,FUNC>(&bbb);
	ans = f2(33);
	std::cout << "12+23=" << ans << std::endl;

	//stdcall を呼ぶには、 registstdcall を使ってください。
}

*/