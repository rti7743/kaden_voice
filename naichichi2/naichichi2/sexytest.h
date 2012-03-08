#pragma once

//テストをやらない無効化するかどうか。
//#define SEXYTEST_DISABLE
//重いテストを無効化するかどうか。
//#define SEXYTEST_DISABLE_HEAVYTEST

//private もテストできるようにする魔法の言葉.
#define SEXYTEST_TEST_FRIEND template<typename _SEXYTEST> friend struct SexyTestUnit; \
	friend class SEXYHOOKFuncBase;

#ifdef _WINDOWS
	//msvc
	#define SEXYTEST_BREAKPOINT()			{ \
			MSG msg;	\
			BOOL bQuit = PeekMessage(&msg, NULL, WM_QUIT, WM_QUIT, PM_REMOVE);	\
			if (bQuit)	PostQuitMessage((int)msg.wParam);	\
			DebugBreak(); \
		} 
#else
	#define SEXYTEST_BREAKPOINT	{ asm ("int $3") ; }
#endif
#ifdef _WINDOWS
	#define SEXYTEST_TRACE(msg)			{ OutputDebugString(msg); } 
#else
	#define SEXYTEST_TRACE(msg)			{ puts(msg); }
#endif


#ifndef SEXYTEST_FAIL_DO
	//テストが失敗したと起動するか？
	//
	//ログとブレークポイント
	#define SEXYTEST_FAIL_DO(msg)	{ SEXYTEST_TRACE(msg); SEXYTEST_BREAKPOINT(); }
	//ログと無視
	//#define SEXYTEST_FAIL_DO(msg)	{ SEXYTEST_TRACE(msg); }
	//無視
	//#define SEXYTEST_FAIL_DO(msg)  
	//ログと強制終了
	//#define SEXYTEST_FAIL_DO(msg) { SEXYTEST_TRACE(msg); exit(-1); }
#endif


//値ダンプ系 (stlコンテナに対してサービスする)
template < typename _T >
std::string SexyTestDump(const _T& s)
{
	std::stringstream out;
	out << s;
	return out.str();
}

//拙者のC++力ではこれか限界でごさった。 誰か直して欲しいでござる.
#ifdef _VECTOR_
	template < class _T > std::string SexyTestDump(const std::vector<_T>& stl){ return SexyTestDumpSTL(stl.begin(),stl.end()); }
#endif
#ifdef _LIST_
	template < class _T > std::string SexyTestDump(const std::list<_T>& stl){ return SexyTestDumpSTL(stl.begin(),stl.end()); }
#endif

#ifdef _DEQUE_
	template < class _T > std::string SexyTestDump(const std::deque<_T>& stl){ return SexyTestDumpSTL(stl.begin(),stl.end()); }
#endif

#ifdef _MAP_
	template < class _T,class _V > std::string SexyTestDump(const std::map<_T,_V>& stl){ return SexyTestDumpPairSTL(stl.begin(),stl.end()); }
#endif

template < typename _B,typename _T >
std::string SexyTestDumpSTL(const _B& begin ,const  _T& end)
{
	_B it = begin;
	std::stringstream out;
	if (end == it)
	{
		return "[]";
	}
	out << "[" << SexyTestDump(*it) ; ++it;

	for(; it != end ; ++it )
	{
		out << "," << SexyTestDump(*it) ;
	}
	out << "]";
	return out.str();
}

template < typename _B,typename _T >
std::string SexyTestDumpPairSTL(const _B& begin ,const  _T& end)
{
	_B it = begin;
	std::stringstream out;
	if (end == it)
	{
		return "[]";
	}
	out << "[" << SexyTestDump(it->first) << "=>" << SexyTestDump(it->second) ; ++it;

	for(; it != end ; ++it )
	{
		out << "," << SexyTestDump(it->first) << "=>" << SexyTestDump(it->second) ;
	}
	out << "]";
	return out.str();
}

//各種チェック用
#define SEXYTEST_ASSERT(f) {\
		if (!(f)){\
			std::stringstream out;\
			out << __FILE__ << "(" << __LINE__ << "):" << " " << testname << " ASSERT! "<< #f << std::endl;\
			SEXYTEST_FAIL_DO(out.str().c_str());\
		}\
	}
#define SEXYTEST_STOP(msg) {\
		std::stringstream out;\
		out << __FILE__ << "(" << __LINE__ << "):" << " " << testname << " STOP! " << msg << std::endl;\
		SEXYTEST_FAIL_DO(out.str().c_str());\
	}
#define SEXYTEST_EQ(x,y) {\
		auto _sexytest_x = (x);\
		auto _sexytest_y = (y);\
		if (_sexytest_x != _sexytest_y){\
			std::stringstream out;\
			out << __FILE__ << "(" << __LINE__ << "):" << " " << testname << " "<< #x << "(" << SexyTestDump(_sexytest_x) << ") == " << #y << "(" << SexyTestDump(_sexytest_y) << ")"  << std::endl;\
			SEXYTEST_FAIL_DO(out.str().c_str());\
		}\
	}
#define SEXYTEST_NOTEQ(x,y) {\
		auto _sexytest_x = (x);\
		auto _sexytest_y = (y);\
		if (_sexytest_x == _sexytest_y){\
			std::stringstream out;\
			out << __FILE__ << "(" << __LINE__ << "):" << " " << testname << " "<< #x << "(" << SexyTestDump(_sexytest_x) << ") != " << #y << "(" << SexyTestDump(_sexytest_y) << ")"  << std::endl;\
			SEXYTEST_FAIL_DO(out.str().c_str());\
		}\
	}
#define SEXYTEST_CMP(x,OP,y) {\
		auto _sexytest_x = (x);\
		auto _sexytest_y = (y);\
		if (!(_sexytest_x OP _sexytest_y)){\
			std::stringstream out;\
			out << __FILE__ << "(" << __LINE__ << "):" << " " << testname << " "<< #x << "(" << SexyTestDump(_sexytest_x) << ") " << #OP << " " << #y << "(" << SexyTestDump(_sexytest_y) << ")"  << std::endl;\
			SEXYTEST_FAIL_DO(out.str().c_str());\
		}\
	}


	struct SexyTestUnitInterface
	{
		virtual const char* getName() = 0;
		virtual void test() = 0;
	};
	struct SexyTestRunner
	{
		typedef std::list< SexyTestUnitInterface* > SEXYTESTTEST_LIST;
		static SEXYTESTTEST_LIST* getSexyTestTestData()
		{
			static SEXYTESTTEST_LIST testList;
			return &testList;
		}

		static void allrun()
		{
			#ifndef SEXYTEST_DISABLE
				SexyTestRunner runner;
				runner.run();
			#endif
		}
		virtual void run()
		{
			#ifndef SEXYTEST_DISABLE
				SEXYTESTTEST_LIST::const_iterator i = SexyTestRunner::getSexyTestTestData()->begin();
				for( ; i != SexyTestRunner::getSexyTestTestData()->end() ; ++ i )
				{
					runone(*i);
				}
			#endif
		}

		virtual void runone(SexyTestUnitInterface* unit)
		{
			const char* testname = unit->getName();
			try
			{
				unit->test();
			}
			catch(std::string & e)
			{
				SEXYTEST_STOP( "例外 " << e );
			}
			catch(std::exception & e)
			{
				SEXYTEST_STOP( "例外 " << e.what() );
			}
			catch(...)
			{
				SEXYTEST_STOP("不明な例外");
			}
		}
	};

	template<typename _T>
	struct SexyTestUnit : public SexyTestUnitInterface
	{
		SexyTestUnit(bool isHeavyTest,const char* name,bool isToppriority)
		{
	#ifndef SEXYTEST_DISABLE
		testname = name;
		#ifndef SEXYTEST_DISABLE_HEAVYTEST
			//全部のテストをやるモードなので、ヘビーなテストでも追加する
			if (isToppriority)
			{
				SexyTestRunner::getSexyTestTestData()->push_front(this);
			}
			else
			{
				SexyTestRunner::getSexyTestTestData()->push_back(this);
			}
		#else
			if (!isHeavyTest)
			{//ヘビーなテストはやらないモードなので、ライトな奴しかやらない
				if (isToppriority)
				{
					SexyTestRunner::getSexyTestTestData()->push_front(this);
				}
				else
				{
					SexyTestRunner::getSexyTestTestData()->push_back(this);
				}
			}
		#endif //SEXYTEST_DISABLE_HEAVYTEST
	#endif //SEXYTEST_DISABLE
		}
		virtual const char* getName() 
		{
			return testname;
		}
		virtual void test() ;
	private:
		const char *testname;
	};


#define _SEXYTEST_LINE_STRCAT(x,y,z) _SEXYTEST_LINE_STRCAT_(x,y,z)
#define _SEXYTEST_LINE_STRCAT_(x,y,z) x##y##z

#define SEXYTEST(name) SEXYTEST_IMP_(name,false,_SEXYTEST_LINE_STRCAT(SexyTest_sexytest_gid_ , __COUNTER__,__LINE__),false)
#define SEXYTEST_HEAVYTEST(name) SEXYTEST_IMP_(name,true,_SEXYTEST_LINE_STRCAT(SexyTest_sexytest_gid_ , __COUNTER__,__LINE__),false)
#define SEXYTEST_TOP(name) SEXYTEST_IMP_(name,false,_SEXYTEST_LINE_STRCAT(SexyTest_sexytest_gid_ , __COUNTER__,__LINE__),true)
#define SEXYTEST_HEAVYTEST_TOP(name) SEXYTEST_IMP_(name,true,_SEXYTEST_LINE_STRCAT(SexyTest_sexytest_gid_ , __COUNTER__,__LINE__),true)
#define SEXYTEST_IMP_(name,heavy,guid,toppriority) \
	static SexyTestUnit<struct guid> guid(false,#name,toppriority); \
	void SexyTestUnit<struct guid>::test()
