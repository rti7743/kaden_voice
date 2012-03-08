#include "common.h"
#include "ScriptRunner.h"
#include "Speak_Factory.h"
#include "Speak_SpeechPlatform.h"
#include "Speak_GoogleTranslate.h"
#include "Speak_AquesTalk.h"
#include "MainWindow.h"
#include "../tinyxml/tinyxml.h"


Speak_Factory::Speak_Factory()
{
	this->Engine = NULL;
}

Speak_Factory::~Speak_Factory()
{
	delete this->Engine;
}

xreturn::r<bool> Speak_Factory::Create(const std::string & name , MainWindow* poolMainWindow,int rate,int pitch,unsigned int volume,const std::string& botname)
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません
	assert(this->Engine == NULL);

	if (name == "sapi")
	{
		this->Engine = new Speak_SpeechPlatform();
	}
	else if (name == "google")
	{
		this->Engine = new Speak_GoogleTranslate();
	}
	else if (name == "aquestalk")
	{
		this->Engine = new Speak_AquesTalk();
	}
	else
	{
		return xreturn::error("合成音声エンジン" + name + "がありません");
	}

	this->Engine->Create(poolMainWindow);
	this->Engine->Setting(rate,pitch,volume,botname);

	return true;
}

xreturn::r<bool> Speak_Factory::Speak(const std::string & str)
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません
	auto r = this->Engine->Speak(str);
	if (!r) return xreturn::error(r.getError());
	return true;
}


void Speak_Factory::RegistWaitCallback(const CallbackDataStruct * callback)
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません
	this->Engine->RegistWaitCallback(callback);
}

void Speak_Factory::Cancel()
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません
	this->Engine->Cancel();
}

//このコールバックに関連付けられているものをすべて消す
xreturn::r<bool> Speak_Factory::RemoveCallback(const CallbackDataStruct* callback , bool is_unrefCallback)
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません
	auto r = this->Engine->RemoveCallback(callback,is_unrefCallback);
	if (!r) return xreturn::error(r.getError());
	return true;
}


std::list<SpeakSentens*> Speak_Factory::SpeakParse(const std::string& speakString,int rate,int volume,int speed)
{
	class Visitor : public TiXmlVisitor
	{
	public:
		std::list<SpeakSentens*> sentens;
		std::list<SpeakSentens*> stack;
		SpeakSentens DummyNullSentens;

		/// Visit an element.
		virtual bool VisitEnter( const TiXmlElement& element, const TiXmlAttribute* firstAttribute )
		{
			SpeakSentens* lastSentens;
			if (stack.empty() )
			{
				lastSentens = &DummyNullSentens;
			}
			else
			{
				lastSentens = (* (stack.rbegin()));
			}
			SpeakSentens* ss = new SpeakSentens;
			sentens.push_back(ss);
			stack.push_back(ss);

			for ( const TiXmlAttribute* atr = firstAttribute; atr ; atr = atr->Next() )
			{
				if ( strcmp("speed" ,atr->Name() ) == 0 )
				{
					ss->speed = atoi( atr->Value() ) + lastSentens->speed;
				}
				else if ( strcmp("rate" ,atr->Name() ) == 0 )
				{
					ss->rate = atoi( atr->Value() ) + lastSentens->rate;
				}
				else if ( strcmp("volume" ,atr->Name() ) == 0 )
				{
					ss->volume = atoi( atr->Value() );
				}
			}
			return true;
		}

		/// Visit an element.
		virtual bool VisitExit( const TiXmlElement&  )
		{
			stack.pop_back();
			return true;
		}

		/// Visit a text node
		virtual bool Visit( const TiXmlText& text )
		{
			if (stack.empty() )
			{//無視.
				return true;
			}
			(* (stack.rbegin()))->sentens = text.Value();
			return true;
		}
	};
	Visitor v;
	v.DummyNullSentens.rate = rate;
	v.DummyNullSentens.volume = volume;
	v.DummyNullSentens.speed = speed;

	std::string speak = "<V>" + speakString + "</V>";

	TiXmlDocument doc;
	if ( ! doc.Parse( speak.c_str() ) )
	{

	}
	doc.Accept(&v);

	return v.sentens;
}

SEXYTEST("スピーチ内容のパーステスト")
{
	{
		std::list<SpeakSentens*> r = Speak_Factory::SpeakParse("test123",0,0,0);
		CDeleteAll(r);
	}
}