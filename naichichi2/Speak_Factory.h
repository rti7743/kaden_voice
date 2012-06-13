#pragma once

struct ISpeechSpeakInterface
{
	ISpeechSpeakInterface(){}
	virtual ~ISpeechSpeakInterface(){}

	//音声のためのオブジェクトの構築.
	virtual xreturn::r<bool> Create(MainWindow* poolMainWindow) = 0;
	virtual xreturn::r<bool> Setting(int rate,int pitch,unsigned int volume,const std::string& botname) = 0;
	virtual xreturn::r<bool> Speak(const CallbackDataStruct * callback,const std::string & str) = 0;
	virtual xreturn::r<bool> Cancel() = 0;
	virtual xreturn::r<bool> RemoveCallback(const CallbackDataStruct* callback , bool is_unrefCallback) = 0;
};

struct SpeakSentens
{
	SpeakSentens() : rate(0),volume(0),speed(0){}
	std::string sentens;
	int rate;
	int volume;
	int speed;
};

class Speak_Factory
{
public:
	Speak_Factory();
	virtual ~Speak_Factory();

	xreturn::r<bool> Create(const std::string & name , MainWindow* poolMainWindow,int rate,int pitch,unsigned int volume,const std::string& botname);
	xreturn::r<bool> Speak(const CallbackDataStruct * callback,const std::string & str);
	void Cancel();
	xreturn::r<bool> RemoveCallback(const CallbackDataStruct* callback , bool is_unrefCallback);

	static std::list<SpeakSentens*> SpeakParse(const std::string& speakString,int rate,int volume,int speed);
private:
	ISpeechSpeakInterface* Engine;
};
