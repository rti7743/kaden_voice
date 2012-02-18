#ifdef _MSC_VER 
	//for VC++
	#pragma once
#endif
#ifndef ____SEXYHOOK____72
#define ____SEXYHOOK____72
//
//接合部を強引に作成する
//SEXYHOOK
//
//
// by rti
//
//
// new BSD ライセンス / NYSL ライセンス 好きに使えばいいんぢゃない？ ^q^
//

//includeとか定義とか.
#ifdef _WINDOWS_
	//windows
	#include <windows.h>
	#include <imagehlp.h>
	#ifdef _MSC_VER 
		#if _MSC_VER <= 1200 
			typedef int intptr_t; 
			typedef unsigned int uintptr_t; 
		#endif 
	#else 
	#endif 
#else
	//linux
	#include <stdarg.h>
	#include <inttypes.h>
	#include <sys/mman.h>
	#include <string.h>
	typedef void* PROC;
#endif

//強制的にポインタにする(邪道)
#ifdef __GNUC__
	// ... を悪用した技
	static void* SEXYHOOK_DARKCAST(int dummy, ... )
	{
//		return *(void**)(&dummy + 1);
		va_list ap;
		va_start(ap, dummy);
		void* p = va_arg(ap, void*);
		va_end(ap);

		return p;
	}
#else
	//msvc のみ gcc だとエラーがでるけど、アーキテクチャ依存しないので好きだ!
	template<typename _T> static void* SEXYHOOK_DARKCAST(int dummy, _T p)
	{
		return *reinterpret_cast<void**>(&p);
	}
#endif

//ブレークポイントとアサーション
#ifdef __GNUC__
	//gcc
	#define SEXYHOOK_BREAKPOINT	{ asm ("int $3") ; }
	#define SEXYHOOK_ASSERT(f) \
		if (!(f) ) \
		{ \
			asm ("int $3") ; \
		} 
#else
	//msvc
	#define SEXYHOOK_BREAKPOINT	{ DebugBreak(); }
	#define SEXYHOOK_ASSERT(f) \
		if (!(f) ) \
		{ \
			MSG msg;	\
			BOOL bQuit = PeekMessage(&msg, NULL, WM_QUIT, WM_QUIT, PM_REMOVE);	\
			if (bQuit)	PostQuitMessage((int)msg.wParam);	\
			DebugBreak(); \
		} 
#endif

//呼び出し規約
#ifdef __GNUC__
	//gcc
	#define SEXYHOOK_STDCALL			__attribute__ ((stdcall))
	#define SEXYHOOK_CDECL				__attribute__ ((cdecl))
	#define SEXYHOOK_FASTCALL			__attribute__ ((regparm(3)))
	#define SEXYHOOK_FASTCALLN(N)		__attribute__ ((regparm(N)))
	#define SEXYHOOK_CLASS											//__thiscall __cdecl とは違うらしい.
	#define SEXYHOOK_CLASS_STDCALL		__attribute__ ((stdcall))
	#define SEXYHOOK_CLASS_CDECL		__attribute__ ((cdecl))
	#define SEXYHOOK_CLASS_FASTCALL		__attribute__ ((regparm(3)))
	#define SEXYHOOK_CLASS_FASTCALLN(N)	__attribute__ ((regparm(N)))
#else
	//msvc
	#define SEXYHOOK_STDCALL		__stdcall
	#define SEXYHOOK_CDECL			__cdecl
	#define SEXYHOOK_FASTCALL		__fastcall
	#define SEXYHOOK_CLASS										//__thiscall __cdecl とは違うらしい.
	#define SEXYHOOK_CLASS_STDCALL	__stdcall
	#define SEXYHOOK_CLASS_CDECL	__cdecl
	#define SEXYHOOK_CLASS_FASTCALL	__fastcall
#endif

//__LINE__を文字列にするためのマクロ
//参考:http://oshiete1.goo.ne.jp/qa1219076.html
#define SEXYHOOK_LINE_TOSTR(x) SEXYHOOK_LINE_TOSTR_(x)
#define SEXYHOOK_LINE_TOSTR_(x) #x

#define SEXYHOOK_LINE_STRCAT(x,y) SEXYHOOK_LINE_STRCAT_(x,y)
#define SEXYHOOK_LINE_STRCAT_(x,y) x##y

//マクロを再度展開する.
#define SEXYHOOK_REMACRO(MACRO)	MACRO

//クラスメソッドでない場合は、 static 指定にしないといけないので、強引に static文字列を作り出す.
#define SEXYHOOK_STDCALL__STATIC			static
#define SEXYHOOK_CDECL__STATIC				static
#define SEXYHOOK_FASTCALL__STATIC			static
#define SEXYHOOK_FASTCALLN__STATIC			static
#define SEXYHOOK_CLASS__STATIC	
#define SEXYHOOK_CLASS_STDCALL__STATIC	
#define SEXYHOOK_CLASS_CDECL__STATIC	
#define SEXYHOOK_CLASS_FASTCALL__STATIC	
#define SEXYHOOK_CLASS_FASTCALLN__STATIC	

//クラスメソッドの場合は this を。そうでなければ NULL を作成する.
#define SEXYHOOK_STDCALL__THIS			NULL
#define SEXYHOOK_CDECL__THIS			NULL
#define SEXYHOOK_FASTCALL__THIS			NULL
#define SEXYHOOK_FASTCALLN__THIS		NULL
#define SEXYHOOK_CLASS__THIS			this
#define SEXYHOOK_CLASS_STDCALL__THIS	this
#define SEXYHOOK_CLASS_CDECL__THIS		this
#define SEXYHOOK_CLASS_FASTCALL__THIS	this

//関数なのかメソッドなのかで typedef の所を替える.
#define SEXYHOOK_STDCALL__HOOKDEF			
#define SEXYHOOK_CDECL__HOOKDEF			
#define SEXYHOOK_FASTCALL__HOOKDEF			
#define SEXYHOOK_FASTCALLN__HOOKDEF			
#define SEXYHOOK_CLASS__HOOKDEF				SEXYHOOKFuncBase::
#define SEXYHOOK_CLASS_STDCALL__HOOKDEF		SEXYHOOKFuncBase::
#define SEXYHOOK_CLASS_CDECL__HOOKDEF		SEXYHOOKFuncBase::
#define SEXYHOOK_CLASS_FASTCALL__HOOKDEF	SEXYHOOKFuncBase::
#define SEXYHOOK_CLASS_FASTCALLN__HOOKDEF	SEXYHOOKFuncBase::

//const同名
#define SEXYHOOK_STDCALL__CONST			
#define SEXYHOOK_CDECL__CONST			
#define SEXYHOOK_FASTCALL__CONST			
#define SEXYHOOK_FASTCALLN__CONST			
#define SEXYHOOK_CLASS__CONST				const
#define SEXYHOOK_CLASS_STDCALL__CONST		const
#define SEXYHOOK_CLASS_CDECL__CONST			const
#define SEXYHOOK_CLASS_FASTCALL__CONST		const
#define SEXYHOOK_CLASS_FASTCALLN__CONST		const


//SEXYHOOK本体 クラス内クラスとして実装する.
#define SEXYHOOK_BEGIN(RET,CALLTYPE,FUNCADDRESS,ARGS) \
	class SEXYHOOK_LINE_STRCAT(SEXYHOOKFunc,__LINE__) : public SEXYHOOKFuncBase { \
	public: \
		SEXYHOOK_LINE_STRCAT(SEXYHOOKFunc,__LINE__)() \
		{ \
		} \
		void Hook(void* inVCallThisPointer = NULL,void* inFuncAddress2 = NULL) \
		{ \
			*(getSexyhookThisPointer()) =  (uintptr_t)this; \
			void* p = inFuncAddress2; \
			if (!p) p = EvalLoad(#FUNCADDRESS); \
			if (!p) p = SEXYHOOK_DARKCAST(0,(HookDef)FUNCADDRESS); \
			FunctionHookFunction( \
					 p \
					,SEXYHOOK_DARKCAST(0,&SEXYHOOK_LINE_STRCAT(SEXYHOOKFunc,__LINE__)::HookFunction) \
					,SEXYHOOK_DARKCAST(0,&SEXYHOOK_LINE_STRCAT(SEXYHOOKFunc,__LINE__)::CallOrignalFunction) \
					,inVCallThisPointer \
					); \
		} \
		void UnHook() \
		{ \
			FunctionUnHookFunction(); \
		} \
		virtual ~SEXYHOOK_LINE_STRCAT(SEXYHOOKFunc,__LINE__)() \
		{ \
			FunctionUnHookFunction(); \
		} \
		typedef RET ( SEXYHOOK_REMACRO(CALLTYPE##__HOOKDEF)* HookDef) ARGS ; \
		typedef RET ( SEXYHOOK_REMACRO(CALLTYPE##__HOOKDEF)* HookDefConst) ARGS SEXYHOOK_REMACRO(CALLTYPE##__CONST); \
		static uintptr_t* getSexyhookThisPointer() \
		{ \
			static uintptr_t thisSaver = 0; \
			return &thisSaver ; \
		} \
		SEXYHOOK_REMACRO(CALLTYPE##__STATIC) RET  CALLTYPE CallOrignalFunction ARGS \
		{ \
			int mizumashi = 1 ; for(int i = 0 ; i < 1000 ; i++){ mizumashi= mizumashi * i + 1; }; \
			throw 0;\
		} \
		SEXYHOOK_REMACRO(CALLTYPE##__STATIC) RET  CALLTYPE HookFunction ARGS \
		{ \
			SEXYHOOK_LINE_STRCAT(SEXYHOOKFunc,__LINE__)* sexyhookThis = ((SEXYHOOK_LINE_STRCAT(SEXYHOOKFunc,__LINE__)*)(*getSexyhookThisPointer()) ); \
			void * sexyhookOrignalThis = (void*) SEXYHOOK_REMACRO(CALLTYPE##__THIS);

#define SEXYHOOK_END \
	} } SEXYHOOK_LINE_STRCAT(objectFUNCHook,__LINE__);\
	SEXYHOOK_LINE_STRCAT(objectFUNCHook,__LINE__).Hook

#define SEXYHOOK_END_AS() \
	} } 



//一時的にフックをやめる
#define SEXYHOOK_UNHOOK()	SEXYHOOKFuncBase::OriginalGarder sexyhook_temp_orignalgrad(sexyhookThis);  

//オリジナルの this pointer
#define SEXYHOOK_THIS(cast)	((cast) sexyhookOrignalThis )

//同名がいたりして、名前があいまいなった場合の解決策をユーザーに提供する.
#define SEXYHOOK_AUTOCAST_CONST(func)	(HookDefConst)(func)

//DLLをロードする構文。
//あれ、NULL返しているだけに見えるよね。うん。だけどね、ちゃんと動くから安心してほしい。
#define SEXYHOOK_DYNAMICLOAD(DLLNAME,FUNCNAME)	NULL

//関数フックを行う際に書き換える領域のサイズ
typedef  char FUNCTIONHOOK_ASM[20] ;

//関数/メソッドをフックする.
class SEXYHOOKFuncBase
{
	//書き換えたアドレスの先頭
	void*			 OrignalFunctionAddr;
	//書き換えた領域を保持する領域
	FUNCTIONHOOK_ASM OrignalAsm;
	//フックするのに使ったコード
	FUNCTIONHOOK_ASM HookAsm;
	//書き換えるアセンブリのサイズ
	int              HookAsmSize;
	//DLLインスタンス
	void*			 APIHookDLLHandle;

public:
	//オリジナルの関数に一時的に戻す.
	class OriginalGarder
	{
		SEXYHOOKFuncBase* FuncBase;
	public:
		OriginalGarder(SEXYHOOKFuncBase* poolFuncBase)
		{
			this->FuncBase = poolFuncBase;
			this->FuncBase->BeginTempOrginal();
		}
		virtual ~OriginalGarder()
		{
			this->FuncBase->EndTempOrginal();
			this->FuncBase = NULL;
		}
	};

	//一時的にフックをやめる
	void BeginTempOrginal()
	{
		ReplaceFunction(this->OrignalFunctionAddr, this->OrignalAsm ,  NULL ,this->HookAsmSize);
	}
	//もう一度フックする
	void EndTempOrginal()
	{
		ReplaceFunction(this->OrignalFunctionAddr, this->HookAsm ,  NULL ,this->HookAsmSize);
	}


	//関数フックを開始する.
	void FunctionHookFunction(void * inUkeFunctionProc , void * inSemeFunctionProc ,void * inCallOrignalFunctionProc , void * inVCallThisPointer )
	{
		//フックする関数(攻め)が開始するアドレスを求める
		uintptr_t semeFunctionAddr = CalcSemeFunctionAddress(inSemeFunctionProc );
		//フックされる関数(受け)が開始するアドレスを求める
		uintptr_t ukeFunctionAddr =  CalcUkeFunctionAddress(inUkeFunctionProc , inVCallThisPointer);


		//書き換えるマシン語
		//フックされる関数の先頭を書き換えて、フックルーチンへ制御を移すようにする。
		this->HookAsmSize = MakeTrampolineHookAsm( &this->HookAsm , ukeFunctionAddr , semeFunctionAddr) ;
		this->OrignalFunctionAddr = (void*)ukeFunctionAddr;
		ReplaceFunction(this->OrignalFunctionAddr , this->HookAsm , &this->OrignalAsm, this->HookAsmSize );

		//オリジナルの関数を呼び出す機構.
		FUNCTIONHOOK_ASM orginalCallAsm;
		uintptr_t orignalCallFunctionAddr = CalcSemeFunctionAddress(inCallOrignalFunctionProc );
		int orginalCallAsmSize = MakeTrampolineHookAsm( &orginalCallAsm , orignalCallFunctionAddr , ukeFunctionAddr ) ;
		ReplaceFunction( (void*)orignalCallFunctionAddr , orginalCallAsm , NULL,orginalCallAsmSize);

		return ;
	}

	//フックするに使われた構文を見て、
	//SEXYHOOK_APILOAD だったら、dllのロードを仕込む。 evalならぬ、evil。
	void* EvalLoad(const char * inAddress)
	{
		//初期化する.
		this->APIHookDLLHandle = NULL;

		const char * p = inAddress;

		p = strstr(p , "SEXYHOOK_DYNAMICLOAD");
		if (!p) return NULL;

		//第1引数 DLL名
		const char * dll1 = strstr(p , "\"");
		if (!dll1) return NULL;
		const char * dll2 = strstr(dll1+1 , "\"");
		if (!dll2) return NULL;

		//第2引数 関数名
		const char * func1 = strstr(dll2+1 , "\"");
		if (!func1) return NULL;
		const char * func2 = strstr(func1+1 , "\"");
		if (!func2) return NULL;


		//文字列をバッファに複製する.
		char buffer[1024];
		int len = (int)(func2 - inAddress) + 1;
		if (len >= sizeof(buffer) )
		{
			//文字列デカすぎ.
			return NULL;
		}
		memcpy(buffer , inAddress , len );

		//終端埋め込んで、
		buffer[(int)(dll2 - inAddress) ] = '\0';
		buffer[(int)(func2 - inAddress) ] = '\0';
		//先頭の保持
		const char * apiHookDLLName = buffer + (int)(dll1 - inAddress + 1);
		const char * apiHookFuncName = buffer + (int)(func1 - inAddress + 1);
#ifdef _WINDOWS_
		//DLL読み込み
		HMODULE mod = LoadLibraryA(apiHookDLLName);
		if ( mod == NULL )
		{
			//DLLが読み込めません!
			SEXYHOOK_BREAKPOINT;
			return NULL;
		}
		//APIのアドレスを求める.
		void* orignalProc = (void*)GetProcAddress(mod , apiHookFuncName );
		if ( orignalProc == NULL )
		{
			//APIがロードできません。
			SEXYHOOK_BREAKPOINT;
			FreeLibrary(mod);
			return NULL;
		}
		//開放のための記録.
		this->APIHookDLLHandle = (void*)mod;
		
		return orignalProc;
#else
		return NULL;
#endif
	}

	//関数フックをやめる
	void FunctionUnHookFunction()
	{
		if (this->OrignalFunctionAddr == NULL)
		{
			return ;
		}
		ReplaceFunction(this->OrignalFunctionAddr , this->OrignalAsm , NULL,this->HookAsmSize);
		this->OrignalFunctionAddr = NULL;

		//APIをアンロードしなくてはいけない場合は開放する.
		if(this->APIHookDLLHandle)
		{
#ifdef _WINDOWS_
			FreeLibrary((HMODULE)this->APIHookDLLHandle);
#endif
			this->APIHookDLLHandle = NULL;
		}
	}
private:
	//フックした関数のアドレスを返す.
	void* getOrignalFunctionAddr()  const
	{
		return this->OrignalFunctionAddr;
	}

	//フックする関数(攻め)が開始するアドレスを求める
	uintptr_t CalcSemeFunctionAddress(void * inSemeFunctionProc )
	{
		//フック関数が開始される絶対アドレス
		if (*((unsigned char*)inSemeFunctionProc+0) == 0xe9)
		{
			//フック関数も ILT経由で飛んでくる場合
			//0xe9 call [4バイト相対アドレス]          4バイト相対は64bit でも変わらず
			uintptr_t jmpaddress = *((unsigned long*)((unsigned char*)inSemeFunctionProc+1));
			return (((uintptr_t)inSemeFunctionProc) + jmpaddress) + 5;
		}
		else
		{
			//即、プログラム領域に飛んでくる場合
			return (uintptr_t)inSemeFunctionProc ;
		}
	}

	//MSVC++ の vcallを解析する.
	uintptr_t CalcVCall(uintptr_t overraideFunctionAddr , void * inVCallThisPointer )
	{

		//仮想関数の vcallだった場合...
		int vcallhead = 0;
		if (
				*((unsigned char*)overraideFunctionAddr+0) == 0x8B
			&&	*((unsigned char*)overraideFunctionAddr+1) == 0x01
			&&	*((unsigned char*)overraideFunctionAddr+2) == 0xFF
			)
		{
			vcallhead = 3;
		}
		else if (
				*((unsigned char*)overraideFunctionAddr+0) == 0x48
			&&	*((unsigned char*)overraideFunctionAddr+1) == 0x8B
			&&	*((unsigned char*)overraideFunctionAddr+2) == 0x01
			&&	*((unsigned char*)overraideFunctionAddr+3) == 0xFF
			)
		{
			vcallhead = 4;
		}
		else
		{
			//vcallではない.
			return 0;
		}

		int plusAddress = 0;
		if (*((unsigned char*)overraideFunctionAddr+vcallhead ) == 0x20)
		{
			//[[this] + 0] にジャンプ
			plusAddress = 0;
		}
		else if (*((unsigned char*)overraideFunctionAddr+vcallhead ) == 0x60)
		{
			//[[this] + ?] にジャンプ
			plusAddress = (int) *((unsigned char*)overraideFunctionAddr+ (vcallhead + 1) );	//4バイト目の1バイト分が加算する値
		}
		else
		{
			//[[this] + ?] にジャンプを計算出来ませんでした...
			SEXYHOOK_BREAKPOINT;
		}
		//C言語のおせっかいで、ポインタは型分プラスしてしまうので、ポインタのサイズで割っとく.
		plusAddress = plusAddress / sizeof(void*);

		//このような関数に一時的に飛ばされている場合...
		//			vcall:
		//			00402BA0   mov         eax,dword ptr [ecx]
		//			00402BA2   jmp         dword ptr [eax]
		//8B 01 FF 20
		//
		// or
		//
		//004025D0   mov         eax,dword ptr [ecx]
		//004025D2   jmp         dword ptr [eax+4]
		//8B 01 FF 60 04
		if ( inVCallThisPointer == NULL )
		{
			//vcallのフックには、 thisポインタが必要です。
			//SEXYHOOK_CLASS_END_VCALL(thisClass) を利用してください。
			SEXYHOOK_BREAKPOINT;
		}

		/*
			//こういう演算をしたい  inVCallThisPointer = &this;
		_asm
		{
			mov ecx,inVCallThisPointer;
			mov ecx,[ecx];
			mov ecx,[ecx];
			mov overraideFunctionAddr,ecx;
		}

					or 
		_asm
		{
			mov ecx,inVCallThisPointer;
			mov ecx,[ecx];
			mov ecx,[ecx+4];			//+? は定義された関数分 virtualの数だけ増えるよ
			mov overraideFunctionAddr,ecx;
		}
		*/
		//多分こんな感じ,,,泣けてくるキャストだ.
		overraideFunctionAddr = (uintptr_t) *((void**)*((void***)inVCallThisPointer) + plusAddress);
		//そこにあるのは  関数の本体 jmp への命令のはず.
		if (*((unsigned char*)overraideFunctionAddr+0) == 0xe9)
		{
			//ついでなので関数の中を書き換えるため、関数の実体へのアドレスを求める.
			uintptr_t jmpaddress = *((unsigned long*)((unsigned char*)overraideFunctionAddr+1));
			overraideFunctionAddr = (((uintptr_t)overraideFunctionAddr) + jmpaddress) + 5;	//+5は e9 00 00 00 00 (ILTのサイズ)
		}

		return overraideFunctionAddr;
	}


	//フックされる関数(受け)が開始するアドレスを求める
	uintptr_t CalcUkeFunctionAddress(void * inUkeFunctionProc , void * inVCallThisPointer)
	{
		#ifdef __GNUC__
			//gccでは仮想関数のポインタを取得しようとすると、 vtable からの index を返してしまう。
			if ( (uintptr_t)inUkeFunctionProc < 100 )
			{
				//クラスのインスタンス(thisポインタ)が渡されていれば、indexから実体の場所を計算可能。
				if (inVCallThisPointer == NULL)
				{
					//thisがないなら計算不可能なので、とりあえずとめる.
					SEXYHOOK_BREAKPOINT;
				}
				//thisがあれば計算してアドレスを求める.
				//参考: http://d.hatena.ne.jp/Seasons/20090208/1234115944
				uintptr_t* vtable = (uintptr_t*) (*((uintptr_t*)inVCallThisPointer));
				//とりあえず、 (index - 1) / sizeof(void*) でアドレスが求まるみたい.
				//コレであっているのか不安だけど、とりあえず動くよ.
				uintptr_t index = ((uintptr_t)inUkeFunctionProc - 1) / sizeof(void*);

				//vtable から index を計算する.
				inUkeFunctionProc = (void*) (vtable[index] );
			}
		#endif
		
		//フックされる関数の実領域を求める.
		uintptr_t overraideFunctionAddr = 0;
		if (*((unsigned char*)inUkeFunctionProc+0) == 0xe9)
		{
			//フック関数も ILT経由で飛んでくる場合
			//0xe9 call [4バイト相対アドレス]
			uintptr_t jmpaddress = *((unsigned long*)((unsigned char*)inUkeFunctionProc+1));
			overraideFunctionAddr = (((uintptr_t)inUkeFunctionProc) + jmpaddress) + 5;	//+5は e9 00 00 00 00 (ILTのサイズ)
		}
		else
		{
			//即、プログラム領域に飛んでくる場合
			overraideFunctionAddr = (uintptr_t)inUkeFunctionProc;
		}

		//仮想関数の vcallだった場合...
		uintptr_t vcallFunctionAddr = this->CalcVCall(overraideFunctionAddr,inVCallThisPointer);
		if (vcallFunctionAddr != 0)
		{
			return vcallFunctionAddr;
		}
		return overraideFunctionAddr;
	}


	//トランポリンフックの作成
	int MakeTrampolineHookAsm(FUNCTIONHOOK_ASM* outBuffer , uintptr_t inUkeFunctionAddr, uintptr_t inSemeFunctionAddr) const
	{
#if (_WIN64 || __x86_64__)
		//参考 http://www.artonx.org/diary/200809.html
		//     http://hrb.osask.jp/wiki/?faq/asm
		if (inSemeFunctionAddr - inUkeFunctionAddr < 0x80000000)
		{//2G以下のjmp
			// 0xe9 相対アドレス   = 5バイト の命令
			*((unsigned char*)outBuffer+0) = 0xe9;	//近隣ジャンプ JMP
			*((unsigned long*)((unsigned char*)outBuffer+1)) = (unsigned long) (inSemeFunctionAddr - inUkeFunctionAddr - 5);	//-5はjmp命令自信のサイズ

			return 5;
		}
		else
		{//2G以上のjmp
			//Nikolay Igottiさんの方式を利用する. 14バイトの命令
			//push 64bit ; ret; で抜ける。 ret なので 相対ではなく絶対アドレス(inSemeFunctionAddr)を見る。
			//push 64bit がそのままだとできないので、分割して push する。
			//http://blogs.sun.com/nike/entry/long_absolute_jumps_on_amd64
			//http://www.ragestorm.net/blogs/?p=107
			*((unsigned char*)outBuffer+0) = 0x68;	//push imm32, subs 8 from rsp
			*((unsigned long*)((unsigned char*)outBuffer+1)) = 0x00000000ffffffff & inSemeFunctionAddr;

			*((unsigned char*)outBuffer+5) = 0xc7;	//mov imm32, 4(%rsp)
			*((unsigned char*)outBuffer+6) = 0x44;
			*((unsigned char*)outBuffer+7) = 0x24;
			*((unsigned char*)outBuffer+8) = 0x04;
			*((unsigned long*)((unsigned char*)outBuffer+9)) = inSemeFunctionAddr >> 32;

			*((unsigned char*)outBuffer+13) = 0xc3;	//ret

			return 14;
		}
#else
//#elif (_WIN32 || __i386__)
		//i386

		// 0xe9 相対アドレス   = 5バイト の命令
		*((unsigned char*)outBuffer+0) = 0xe9;	//近隣ジャンプ JMP
		*((unsigned long*)((unsigned char*)outBuffer+1)) = (unsigned long) (inSemeFunctionAddr - inUkeFunctionAddr - 5);	//-5はjmp命令自信のサイズ

		return 5;
#endif
	}



	bool ReplaceFunction(void* inAddr , void* inNewSrc ,  void* outOldSrc , int size)
	{
		//割り込むコードを書き込みます。

		if (outOldSrc != NULL)
		{
			//バックアップ
			memcpy(outOldSrc , inAddr , size );
		}

		#ifdef _WINDOWS_
			//書き換え許可
			unsigned long oldProtect = 0;
			VirtualProtect( inAddr , size , PAGE_EXECUTE_READWRITE , &oldProtect);
			//書き換え
			memcpy(inAddr , inNewSrc , size );
			//属性を元に戻す
			VirtualProtect( inAddr , size , oldProtect , &oldProtect);
		#else
			//mprotect に渡す値は、ページ境界(4K)に合わせる
			//http://d.hatena.ne.jp/kanbayashi/20081005/p2
			void *pageAddr = (void*)(((uintptr_t)inAddr) & 0xFFFFF000);
			//書き換え許可
			mprotect( pageAddr, 0x1000 , PROT_READ | PROT_WRITE | PROT_EXEC);
			//書き換え
			memcpy(inAddr , inNewSrc , size );
			//もとに戻したいんだけどどうすればいいの？
		#endif
		return true;
	}
};

#endif //____SEXYHOOK____72
