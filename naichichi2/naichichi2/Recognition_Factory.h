#pragma once


struct ISpeechRecognitionInterface
{
	ISpeechRecognitionInterface(){}
	virtual ~ISpeechRecognitionInterface(){}

	virtual xreturn::r<bool> Create(MainWindow* poolMainWindow) = 0;
	//呼びかけを設定します。
	//設定したあと、 CommitRule() てしてね。
	virtual xreturn::r<bool> SetYobikake(const std::list<std::string> & yobikakeList) = 0;
	//認識結果で不感染なものを捨てる基準値を設定します。
	virtual xreturn::r<bool> SetRecognitionFilter(double temporaryRuleConfidenceFilter,double yobikakeRuleConfidenceFilter,double basicRuleConfidenceFilter,bool dictation_Filter) = 0;
	//コマンドに反応する音声認識ルールを構築します
	virtual xreturn::r<bool> AddCommandRegexp(CallbackDataStruct & callback,const std::string & str) = 0;
	//テンポラリルールに反応する音声認識ルールを構築します
	virtual xreturn::r<bool> AddTemporaryRegexp(CallbackDataStruct & callback,const std::string & str) = 0;
	//テンポラリルールをすべてクリアします
	virtual xreturn::r<bool> ClearTemporary() = 0;
	//構築したルールを音声認識エンジンにコミットします。
	virtual xreturn::r<bool> CommitRule() = 0;
};

class Recognition_Factory
{
public:
	Recognition_Factory();
	virtual ~Recognition_Factory();

	xreturn::r<bool> Recognition_Factory::Create(const std::string & name , MainWindow* poolMainWindow,std::list<std::string> yobikakeListArray,double temporaryRuleConfidenceFilter,double yobikakeRuleConfidenceFilter,double basicRuleConfidenceFilter,bool dictation_Filter);
	xreturn::r<bool> Recognition_Factory::AddCommandRegexp(CallbackDataStruct & callback ,const std::string p1);
	xreturn::r<bool> Recognition_Factory::AddTemporaryRegexp(CallbackDataStruct & callback ,const std::string p1);
	xreturn::r<bool> Recognition_Factory::CommitRule();
	xreturn::r<bool> Recognition_Factory::ClearTemporary();

private:
	ISpeechRecognitionInterface* Engine;
};
