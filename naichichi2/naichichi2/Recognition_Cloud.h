#pragma once;
#include "common.h"


extern "C"{
	#include <julius/juliuslib.h>
};

class Recognition_Cloud: public ISpeechRecognitionInterface
{
public:
	Recognition_Cloud();
	virtual ~Recognition_Cloud();

	//構築
	virtual bool Create(MainWindow* poolMainWindow) ;
	//呼びかけを設定します。
	//設定したあと、 CommitRule() てしてね。
	virtual bool SetYobikake(const std::list<std::string> & yobikakeList) ;
	virtual bool SetCancel(const std::list<std::string> & cancelList) ;
	//認識結果で不確実なものを捨てる基準値を設定します。
	virtual bool SetRecognitionFilter(double temporaryRuleConfidenceFilter) ;
	//音声データを保存するディレクトリ
	virtual bool SetLogDirectory(const std::string& logdir);

	//コマンドに反応する音声認識ルールを構築します
	virtual bool AddCommandRegexp(const CallbackDataStruct * callback,const std::string & str) ;
	//テンポラリルールに反応する音声認識ルールを構築します
	virtual bool AddTemporaryRegexp(const CallbackDataStruct* callback,const std::string & str) ;
	//テンポラリルールをすべてクリアします
	virtual bool ClearTemporary() ;
	//構築したルールを音声認識エンジンにコミットします。
	virtual bool CommitRule() ;
	//このコールバックに関連付けられているものをすべて消す
	virtual bool RemoveCallback(const CallbackDataStruct* callback , bool is_unrefCallback) ;

	int adin_callback_file(SP16 *now, int len, Recog *recog);
private:
	void JuliusStop();
	bool JuliusStart();
	void ThreadMain();
	void Reaction(const std::string &recogResult);

	int TemporaryRuleCount ;
	bool IsNeedUpdateRule;

	std::string LogDirectory;


	struct CallbackRegexpStruct
	{
		const CallbackDataStruct* callback;
		std::string regexp;

		CallbackRegexpStruct(const CallbackDataStruct* callback,const std::string& regexp)
		{
			this->callback = callback;
			this->regexp = regexp;
		}
	};
	std::list<CallbackRegexpStruct*> RegexpList;
	std::list<CallbackRegexpStruct*> RegexpTemporaryList;

	//マイクから読み込んで実行するjulius
	Jconf *jconf;
	Recog *recog;

	//認識したwaveファイル
	std::vector<SP16> WaveFileData;
	std::string CloudServer;

	//juliusをスレッドで動作させます。
	boost::thread *Thread;

	MainWindow* PoolMainWindow;
};
