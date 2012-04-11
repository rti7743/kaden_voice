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
#include <bfd.h>		//行番号まで取り出す
						//入っていないなら yum install binutils-devel とかしてね!
						// #include <bfd.h> 自体をコメントアウトすると機能全体をOFFにできるよ!!
#endif


namespace xreturn
{


class BackTrace
{
#ifdef _MSC_VER
	//dll読み込みヘルパー
	class LoadLibraryHelper
	{
	private:
		//DLL インスタンス.
		HMODULE DllInstance;

	public:
		LoadLibraryHelper()
		{
			this->DllInstance = NULL;
		}
		virtual ~LoadLibraryHelper()
		{
			if (this->DllInstance != NULL)
			{
				::FreeLibrary(this->DllInstance);
				this->DllInstance = NULL;
			}
		}
		bool Load( const char* inDLLName )//std読んでいない化石環境とかのために const char* で作る.
		{
			if (this->DllInstance != NULL)
			{
				return false;
			}
			this->DllInstance = ::LoadLibraryA(inDLLName);
			return this->DllInstance != NULL;
		}
		FARPROC GetProcAddress(const char* inProcName)
		{
			if (!this->DllInstance)
			{
				return NULL;
			}
			return ::GetProcAddress(this->DllInstance,inProcName);
		}
	};

	//スタックトレースAPI
	typedef USHORT (WINAPI *RtlCaptureStackBackTraceDef) (ULONG FramesToSkip,ULONG FramesToCapture,PVOID *BackTrace,__out_opt  PULONG BackTraceHash);
	RtlCaptureStackBackTraceDef RtlCaptureStackBackTraceProc;

	//シンボルエンジンの準備ができているか
	bool IsSymbolEngineReady;
	//プロセスハンドル
	HANDLE	Process;

	#if (_WIN64 || __x86_64__)
	//アドレスからモジュールを求める
	typedef BOOL (WINAPI *SymGetModuleInfo64Def) (HANDLE hProcess, DWORD64 dwAddr,  PIMAGEHLP_MODULE64 ModuleInfo );
	SymGetModuleInfo64Def SymGetModuleInfo64Proc;
	//アドレスからシンボルを求める
	typedef BOOL (WINAPI *SymGetSymFromAddr64Def) (HANDLE hProcess,DWORD64 Address,PDWORD64 Displacement,PIMAGEHLP_SYMBOL64 Symbol);
	SymGetSymFromAddr64Def SymGetSymFromAddr64Proc;
	//アドレスからファイルと行番号を求める
	typedef BOOL (WINAPI *SymGetLineFromAddr64Def) (HANDLE hProcess,  DWORD64 Address,  PDWORD64 Displacement,  PIMAGEHLP_LINE64 Line);
	SymGetLineFromAddr64Def SymGetLineFromAddr64Proc;
	#else
	//アドレスからモジュールを求める
	typedef BOOL (WINAPI *SymGetModuleInfoDef) (HANDLE hProcess, DWORD dwAddr,  PIMAGEHLP_MODULE ModuleInfo );
	SymGetModuleInfoDef SymGetModuleInfoProc;
	//アドレスからシンボルを求める
	typedef BOOL (WINAPI *SymGetSymFromAddrDef) (HANDLE hProcess,DWORD Address,PDWORD Displacement,PIMAGEHLP_SYMBOL Symbol);
	SymGetSymFromAddrDef SymGetSymFromAddrProc;
	//アドレスからファイルと行番号を求める
	typedef BOOL (WINAPI *SymGetLineFromAddrDef) (HANDLE hProcess,  DWORD dwAddr,  PDWORD pdwDisplacement,  PIMAGEHLP_LINE Line);
	SymGetLineFromAddrDef SymGetLineFromAddrProc;
	#endif


	//シンボルエンジンのオプション
	typedef BOOL (WINAPI *SymSetOptionsDef) ( DWORD SymOptions );
	SymSetOptionsDef SymSetOptionsProc;

	//シンボルエンジンの初期化
	typedef BOOL (WINAPI *SymInitializeDef) (HANDLE hProcess,      PSTR UserSearchPath,   BOOL fInvadeProcess  );
	SymInitializeDef SymInitializeProc;

	//シンボルエンジンの終了
	typedef BOOL (WINAPI *SymCleanupDef) (HANDLE hProcess);
	SymCleanupDef SymCleanupProc;
  
	LoadLibraryHelper Kernel32Librsry;
	LoadLibraryHelper DbgHelpLibrsry;

	BackTrace()
	{
		this->IsSymbolEngineReady = FALSE;
		this->RtlCaptureStackBackTraceProc = NULL;
		this->Process = NULL;
		this->SymSetOptionsProc = NULL;
		this->SymInitializeProc = NULL;
		this->SymCleanupProc = NULL;
		#if (_WIN64 || __x86_64__)
			this->SymGetModuleInfo64Proc = NULL;
			this->SymGetSymFromAddr64Proc = NULL;
			this->SymGetLineFromAddr64Proc = NULL;
		#else
			this->SymGetModuleInfoProc = NULL;
			this->SymGetSymFromAddrProc = NULL;
			this->SymGetLineFromAddrProc = NULL;
		#endif
		if ( ! this->Kernel32Librsry.Load("kernel32.dll") )
		{
			return ;
		}
		if ( ! this->DbgHelpLibrsry.Load("dbghelp.dll") )
		{
			return ;
		}
		this->RtlCaptureStackBackTraceProc = (RtlCaptureStackBackTraceDef)this->Kernel32Librsry.GetProcAddress("RtlCaptureStackBackTrace");
		#if (_WIN64 || __x86_64__)
			this->SymGetModuleInfo64Proc = (SymGetModuleInfo64Def)this->DbgHelpLibrsry.GetProcAddress("SymGetModuleInfo64");
			this->SymGetSymFromAddr64Proc = (SymGetSymFromAddr64Def)this->DbgHelpLibrsry.GetProcAddress("SymGetSymFromAddr64");
			this->SymGetLineFromAddr64Proc = (SymGetLineFromAddr64Def)this->DbgHelpLibrsry.GetProcAddress("SymGetLineFromAddr64");
		#else
			this->SymGetModuleInfoProc = (SymGetModuleInfoDef)this->DbgHelpLibrsry.GetProcAddress("SymGetModuleInfo");
			this->SymGetSymFromAddrProc = (SymGetSymFromAddrDef)this->DbgHelpLibrsry.GetProcAddress("SymGetSymFromAddr");
			this->SymGetLineFromAddrProc = (SymGetLineFromAddrDef)this->DbgHelpLibrsry.GetProcAddress("SymGetLineFromAddr");
		#endif
		this->SymSetOptionsProc = (SymSetOptionsDef)this->DbgHelpLibrsry.GetProcAddress("SymSetOptions");
		this->SymInitializeProc = (SymInitializeDef)this->DbgHelpLibrsry.GetProcAddress("SymInitialize");
		this->SymCleanupProc = (SymCleanupDef)this->DbgHelpLibrsry.GetProcAddress("SymCleanup");

		if (   !this->RtlCaptureStackBackTraceProc 
			&& !this->SymSetOptionsProc
			&& !this->SymInitializeProc
			&& !this->SymCleanupProc
			#if (_WIN64 || __x86_64__)
			&& !this->SymGetModuleInfo64Proc
			&& !this->SymGetSymFromAddr64Proc
			&& !this->SymGetLineFromAddr64Proc
			#else
			&& !this->SymGetModuleInfoProc
			&& !this->SymGetSymFromAddrProc
			&& !this->SymGetLineFromAddrProc
			#endif
			)
		{
			return ;
		}

		//プロセスを記録.
		this->Process = ::GetCurrentProcess();


		//シンボルエンジンの初期化.
		//行番号付きのデータをロードしてねとお願いする.
		this->SymSetOptionsProc(SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES | SYMOPT_UNDNAME);
        if (this->SymInitializeProc(this->Process, NULL, TRUE))
        {
			//シンボルエンジン準備完了
			this->IsSymbolEngineReady = true;
        }

		return ;
	}
public:
	virtual ~BackTrace()
	{
		if (this->IsSymbolEngineReady)
		{
			this->SymCleanupProc(this->Process);
			this->IsSymbolEngineReady = false;
		}
	}
#elif __GNUC__
	//シンボルエンジンの準備ができているか
	bool		IsSymbolEngineReady ;
	#ifdef __BFD_H_SEEN__
		bfd*		Abfd;
		asymbol**	Symbols;
		int			NSymbols;
		asection*	Section;
	#endif

	BackTrace()
	{
		#ifdef __BFD_H_SEEN__
			this->IsSymbolEngineReady = false;
			this->Abfd = NULL;
			this->Symbols = NULL;
			this->NSymbols = 0;

			//see http://0xcc.net/blog/archives/000073.html
			this->Abfd = bfd_openr("/proc/self/exe", NULL);
			if (!this->Abfd)
			{
				return ;
			}
			bfd_check_format(this->Abfd, bfd_object);

			int size = bfd_get_symtab_upper_bound(this->Abfd);
			if (size <= 0)
			{
				return ;
			}
			this->Symbols = (asymbol**) malloc(size);
			if (!this->Symbols)
			{
				return ;
			}
			this->NSymbols = bfd_canonicalize_symtab(this->Abfd, this->Symbols);
			if (!this->NSymbols)
			{
				return ;
			}
			this->Section = bfd_get_section_by_name(this->Abfd, ".debug_info");
			if (!this->Section)
			{
				return ;
			}

			//シンボルエンジンの初期化完了
			this->IsSymbolEngineReady = true;
		#else
			//シンボルエンジンは利用できない!!
			this->IsSymbolEngineReady = false;
		#endif
	}

public:
	virtual ~BackTrace()
	{
		#ifdef __BFD_H_SEEN__
			if (this->Symbols)
			{
				free(this->Symbols);
				this->Symbols = NULL;
			}
			if (this->Abfd)
			{
				bfd_close (this->Abfd);
				this->Abfd = NULL;
			}
		#endif
	}
#endif

public:

#ifdef _MSC_VER
	//スタックトレースを取得する.
	int backtrace(void** buffer , int n) const
	{
		if ( ! this->RtlCaptureStackBackTraceProc )
		{
			//ロードされていない。
			return 0;
		}

		if (n >= 63)
		{
			n = 62;
		}
        return (int) this->RtlCaptureStackBackTraceProc(0,n,buffer,NULL);
	}

	#if (_WIN64 || __x86_64__) //for64bit
	//シンボルの解決
	void addressToSymbolString(void* address ,char * outBuffer , int len) const
	{
		if ( ! this->IsSymbolEngineReady )
		{
			//シンボルエンジンが準備できていない.
			#if __STDC_WANT_SECURE_LIB__
			_snprintf_s(outBuffer ,len , _TRUNCATE , "0x%p @ ??? @ ??? @ ???:???" ,address );
			#else
			_snprintf(outBuffer , len , "0x%p @ ??? @ ??? @ ???:???" ,address );
			#endif
			return ;
		}

		//モジュール名
		IMAGEHLP_MODULE64 imageModule = { sizeof(IMAGEHLP_MODULE64) };
		BOOL r = this->SymGetModuleInfo64Proc(this->Process ,(DWORD64) address , &imageModule);
		if (!r)
		{
			#if __STDC_WANT_SECURE_LIB__
			_snprintf_s(outBuffer ,len , _TRUNCATE , "0x%p @ ??? @ ??? @ ???:???" ,address );
			#else
			_snprintf(outBuffer , len , "0x%p @ ??? @ ??? @ ???:???" ,address );
			#endif
			return ;
		}

		//シンボル情報格納バッファ.
		IMAGEHLP_SYMBOL64 * imageSymbol;
		char buffer[MAX_PATH + sizeof(IMAGEHLP_SYMBOL64) ] = {0};
		imageSymbol = (IMAGEHLP_SYMBOL64*)buffer;
		imageSymbol->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL64);
		imageSymbol->MaxNameLength = MAX_PATH;

		//関数名の取得...
		DWORD64 disp = 0;
		r = this->SymGetSymFromAddr64Proc(this->Process , (DWORD64)address , &disp , imageSymbol );
		if (!r)
		{//関数名がわかりません.
			#if __STDC_WANT_SECURE_LIB__
			_snprintf_s(outBuffer ,len , _TRUNCATE , "0x%p @ %s @ ??? @ ???:???" ,address,imageModule.ModuleName );
			#else
			_snprintf(outBuffer , len , "0x%p @ %s @ ??? @ ???:???" ,address,imageModule.ModuleName );
			#endif
			return ;
		}

		//行番号の取得
		IMAGEHLP_LINE64 line ={sizeof(IMAGEHLP_LINE64)};
		r = this->SymGetLineFromAddr64Proc(this->Process ,(DWORD64) address , &disp , &line);
		if (!r)
		{//行番号が分かりません
			#if __STDC_WANT_SECURE_LIB__
			_snprintf_s(outBuffer ,len , _TRUNCATE , "0x%p @ %s @ %s @ %s+%d" ,address,
				imageModule.ModuleName , imageSymbol->Name, imageSymbol->Name,(int) ((char*)address - (char*)line.Address) );
			#else
			_snprintf(outBuffer , len , "0x%p @ %s @ %s @ %s+%d" ,address,imageModule.ModuleName , 
				imageModule.ModuleName , imageSymbol->Name, imageSymbol->Name,(int) ((char*)address - (char*)line.Address) );
			#endif
			return ;
		}

		//行番号がわかりました.
		#if __STDC_WANT_SECURE_LIB__
		_snprintf_s(outBuffer , len ,_TRUNCATE, "0x%p @ %s @ %s @ %s:%d" ,address,imageModule.ModuleName , imageSymbol->Name , line.FileName , line.LineNumber);
		#else
		_snprintf(outBuffer , len , "0x%p @ %s @ %s @ %s:%d" ,address,imageModule.ModuleName , imageSymbol->Name , line.FileName , line.LineNumber);
		#endif
	}
	#else //for 32bit
	//シンボルの解決
	void addressToSymbolString(const void* address ,char * outBuffer , int len) const
	{
		if ( ! this->IsSymbolEngineReady )
		{
			//シンボルエンジンが準備できていない.
			#if __STDC_WANT_SECURE_LIB__
			_snprintf_s(outBuffer ,len , _TRUNCATE , "0x%p @ ??? @ ??? @ ???:???" ,address );
			#else
			_snprintf(outBuffer , len , "0x%p @ ??? @ ??? @ ???:???" ,address );
			#endif
			return ;
		}

		//モジュール名
		IMAGEHLP_MODULE imageModule = { sizeof(IMAGEHLP_MODULE) };
		BOOL r = this->SymGetModuleInfoProc(this->Process ,(DWORD) address , &imageModule);
		if (!r)
		{
			#if __STDC_WANT_SECURE_LIB__
			_snprintf_s(outBuffer ,len , _TRUNCATE , "0x%p @ ??? @ ??? @ ???:???" ,address );
			#else
			_snprintf(outBuffer , len , "0x%p @ ??? @ ??? @ ???:???" ,address );
			#endif
			return ;
		}

		//シンボル情報格納バッファ.
		IMAGEHLP_SYMBOL * imageSymbol;
		char buffer[MAX_PATH + sizeof(IMAGEHLP_SYMBOL) ] = {0};
		imageSymbol = (IMAGEHLP_SYMBOL*)buffer;
		imageSymbol->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL);
		imageSymbol->MaxNameLength = MAX_PATH;

		//関数名の取得...
		DWORD disp = 0;
		r = this->SymGetSymFromAddrProc(this->Process , (DWORD)address , &disp , imageSymbol );
		if (!r)
		{//関数名がわかりません.
			#if __STDC_WANT_SECURE_LIB__
			_snprintf_s(outBuffer ,len , _TRUNCATE , "0x%p @ %s @ ??? @ ???:???" ,address,imageModule.ModuleName );
			#else
			_snprintf(outBuffer , len , "0x%p @ %s @ ??? @ ???:???" ,address,imageModule.ModuleName );
			#endif
			return ;
		}

		//行番号の取得
		IMAGEHLP_LINE line ={sizeof(IMAGEHLP_LINE)};
		r = this->SymGetLineFromAddrProc(this->Process ,(DWORD) address , &disp , &line);
		if (!r)
		{//行番号が分かりません
			#if __STDC_WANT_SECURE_LIB__
			_snprintf_s(outBuffer ,len , _TRUNCATE , "0x%p @ %s @ %s @ %s+%d" ,address,
				imageModule.ModuleName , imageSymbol->Name, imageSymbol->Name,(int) ((char*)address - (char*)line.Address) );
			#else
			_snprintf(outBuffer , len , "0x%p @ %s @ %s @ %s+%d" ,address,imageModule.ModuleName , 
				imageModule.ModuleName , imageSymbol->Name, imageSymbol->Name,(int) ((char*)address - (char*)line.Address) );
			#endif
			return ;
		}

		//行番号がわかりました.
		#if __STDC_WANT_SECURE_LIB__
		_snprintf_s(outBuffer , len ,_TRUNCATE, "0x%p @ %s @ %s @ %s:%d" ,address,imageModule.ModuleName , imageSymbol->Name , line.FileName , line.LineNumber);
		#else
		_snprintf(outBuffer , len , "0x%p @ %s @ %s @ %s:%d" ,address,imageModule.ModuleName , imageSymbol->Name , line.FileName , line.LineNumber);
		#endif
	}
	#endif //(_WIN64 || __x86_64__)
#elif __GNUC__
	//スタックトレースを取得する.
	int backtrace(void** buffer , int n) const
	{
		return ::backtrace(buffer , n);
	}

	//シンボルの解決
	void addressToSymbolString(void* address ,char * outBuffer , int len) const
	{
		//backtrace_symbols で一発、、、なんてことをすると demangle されない場合があるそうな。
		//だから、dladdr でバラしていく。
		//see http://d.hatena.ne.jp/syuu1228/20100215/1266262848
		Dl_info info;
		if (! dladdr(address, &info) ) 
		{
			snprintf(outBuffer ,len , "0x%p @ ??? @ ??? @ ???:???" ,address );
			return ;
		}
		if (!info.dli_sname)
		{
			snprintf(outBuffer ,len , "0x%p @ %s @ ??? @ ???:???" ,address , info.dli_fname );
			return ;
		}
		if (!info.dli_saddr)
		{
			snprintf(outBuffer ,len , "0x%p @ %s @ %s @ ???:???" ,address , info.dli_fname , info.dli_sname );
			return ;
		}

		//デマングルして関数名を読める形式に
		int status = 0;
		char * demangled = abi::__cxa_demangle(info.dli_sname,0,0,&status);

		//シンボルエンジンは使えるの？
		if (!this->IsSymbolEngineReady)
		{
			snprintf(outBuffer ,len , "0x%p @ %s @ %s @ %s+0x%p" ,
					address , info.dli_fname , (demangled ? demangled : info.dli_sname),
											   (demangled ? demangled : info.dli_sname), 
											   (unsigned int) ((char *)address - (char *)info.dli_saddr) );
			free(demangled);
			return ;
		}
		#ifdef __BFD_H_SEEN__
			//ファイル名と行数を求める.
			const char* filename = NULL;
			const char* functionname = NULL;
			unsigned int line = 0;

			int found = bfd_find_nearest_line(this->Abfd, this->Section, this->Symbols,
												(long)address,
												&filename,
												&functionname,
												&line);
			if (found && filename != NULL && functionname != NULL) 
			{
				snprintf(outBuffer ,len , "0x%p @ %s @ %s @ %s+0x%p" ,
						address , info.dli_fname , (demangled ? demangled : info.dli_sname),
												    (demangled ? demangled : info.dli_sname),
											        (unsigned int) ((char *)address - (char *)info.dli_saddr) );
				free(demangled);
				return ;
			}

			snprintf(outBuffer ,len , "0x%p @ %s @ %s @ %s:%d" ,
					address , info.dli_fname , (demangled ? demangled : info.dli_sname), filename , line );
			free(demangled);
		#endif

		return ;
	}

#endif
	//シンボルをまとめて解決
	void addressToFullSymbolString(const void** address ,int size , char * outBuffer , int len) const
	{
		int writesize = 0;

		int i = 0;
		for( i = 0 ; i < size ; i ++)
		{
			const void * p = address[i];
			if (p == NULL)
			{
				break;
			}

			this->addressToSymbolString(p,outBuffer + writesize, len - writesize);
			writesize += (int)strlen(outBuffer + writesize);

			if (len - writesize >= 2)
			{
				#if __STDC_WANT_SECURE_LIB__
				strncat_s(outBuffer + writesize , len - writesize , "\r\n",2);
				#else
				strncat(outBuffer + writesize , "\r\n",2);
				#endif
				writesize += 2;
			}

			if (len <= writesize)
			{
				break;
			}
		}
	}

	//singletonの取得
	static const BackTrace* Get()
	{
		static BackTrace s;
		return &s;
	}
};


//エラーコードと std::string もいけるようにした例外クラス
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
		this->StackBufferSize = BackTrace::Get()->backtrace(this->StackBuffer , 50);
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
		char stacktraceBuffer[1024];
		BackTrace::Get()->addressToFullSymbolString((const void**)this->StackBuffer , this->StackBufferSize , stacktraceBuffer , 1024);

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
			this->message = std::string("can not find error. error code:") + num2str(lasterror);
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
		DWORD lasterror = errno;
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
	virtual ~errnoError()
	{
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

