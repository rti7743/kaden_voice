#pragma once


struct ISpeechRecognitionInterface
{
	ISpeechRecognitionInterface(){}
	virtual ~ISpeechRecognitionInterface(){}

	virtual bool Create(MainWindow* poolMainWindow) = 0;
	//呼びかけを設定します。
	//設定したあと、 CommitRule() てしてね。
	virtual bool SetYobikake(const std::list<std::string> & yobikakeList) = 0;
	virtual bool SetCancel(const std::list<std::string> & cancelList) = 0;
	//認識結果で不確実なものを捨てる基準値を設定します。
	virtual bool SetRecognitionFilter(double temporaryRuleConfidenceFilter) = 0;
	//音声データを保存するディレクトリ
	virtual bool SetLogDirectory(const std::string& logdir) = 0;
	//コマンドに反応する音声認識ルールを構築します
	virtual bool AddCommandRegexp(const CallbackDataStruct * callback,const std::string & str) = 0;
	//テンポラリルールに反応する音声認識ルールを構築します
	virtual bool AddTemporaryRegexp(const CallbackDataStruct * callback,const std::string & str) = 0;
	//テンポラリルールをすべてクリアします
	virtual bool ClearTemporary() = 0;
	//構築したルールを音声認識エンジンにコミットします。
	virtual bool CommitRule() = 0;
	//このコールバックに関連付けられているものをすべて消す
	virtual bool RemoveCallback(const CallbackDataStruct* callback , bool is_unrefCallback) = 0;
};

class Recognition_Factory
{
public:
	Recognition_Factory();
	virtual ~Recognition_Factory();

	bool Create(const std::string & name , MainWindow* poolMainWindow,std::list<std::string> yobikakeListArray,std::list<std::string> cancelListArray,double temporaryRuleConfidenceFilter,const std::string& logdir);
	bool AddCommandRegexp(const CallbackDataStruct * callback ,const std::string p1);
	bool AddTemporaryRegexp(const CallbackDataStruct * callback ,const std::string p1);
	bool SetLogDirectory(const std::string& logdir);
	bool CommitRule();
	bool ClearTemporary();
	bool RemoveCallback(const CallbackDataStruct* callback , bool is_unrefCallback);

private:
	ISpeechRecognitionInterface* Engine;
};
