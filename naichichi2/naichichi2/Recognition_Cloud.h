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
	virtual xreturn::r<bool> Create(MainWindow* poolMainWindow) ;
	//呼びかけを設定します。
	//設定したあと、 CommitRule() てしてね。
	virtual xreturn::r<bool> SetYobikake(const std::list<std::string> & yobikakeList) ;
	virtual xreturn::r<bool> SetCancel(const std::list<std::string> & cancelList) ;
	//認識結果で不確実なものを捨てる基準値を設定します。
	virtual xreturn::r<bool> SetRecognitionFilter(double temporaryRuleConfidenceFilter) ;
	//音声データを保存するディレクトリ
	virtual xreturn::r<bool> SetLogDirectory(const std::string& logdir);

	//コマンドに反応する音声認識ルールを構築します
	virtual xreturn::r<bool> AddCommandRegexp(const CallbackDataStruct * callback,const std::string & str) ;
	//テンポラリルールに反応する音声認識ルールを構築します
	virtual xreturn::r<bool> AddTemporaryRegexp(const CallbackDataStruct* callback,const std::string & str) ;
	//テンポラリルールをすべてクリアします
	virtual xreturn::r<bool> ClearTemporary() ;
	//構築したルールを音声認識エンジンにコミットします。
	virtual xreturn::r<bool> CommitRule() ;
	//このコールバックに関連付けられているものをすべて消す
	virtual xreturn::r<bool> RemoveCallback(const CallbackDataStruct* callback , bool is_unrefCallback) ;
	//メディア情報をアップデートします。
	virtual xreturn::r<bool> UpdateMedia(const std::string& name ,const std::list<std::string>& list ) ;

	int adin_callback_file(SP16 *now, int len, Recog *recog);
private:
	void JuliusStop();
	xreturn::r<bool> JuliusStart();
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
