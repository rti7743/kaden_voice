#pragma once;
#include "common.h"
#include "Recognition_JuliusPlusRule.h"
#include "XLMachineLearningLibliear.h"


extern "C"{
	#include <julius/juliuslib.h>
};

class Recognition_JuliusPlus: public ISpeechRecognitionInterface
{
public:
	//1ノード単位 
	//ノードとは、 dict の単語単位、1つの単語単位だと思ってください。
	//単語が集まって、文章ができます。
	struct OneNode
	{

		int dictNumber;

		float cmscore;	//確率 juliusがいう「ライバルに比べた」単語信頼度。
						//ライバルに比べたというのがミソで、ライバルがほとんどいない単語だと
						//ふつーに 1.0 (100%)になってしまうという不思議仕様の使えない子。ダメダメすぐる。
						//julius plusでは、gomi単語をばらまいてライバルを擬似的に作るという荒業をおこなっている。
		float avgscore;	//-walign で取得できる単語のスコア。 尤度？
		int begin_frame;
		int end_frame;

		const Recognition_JuliusPlusRule* rule;
		const Recognition_JuliusPlusRule::wordnode* wordnode;
		float plus_node_score; // avgscore / (end_frame-begin_frame) にしたもの。
	};
	//1文
	struct OncSentence
	{
		std::list<OneNode*>        nodes;

		float score;				//julus公式のスコア
		float plus_sentence_score;	//いろいろ合算して、そこそこ信頼出来る数字にしたもの
		int   all_frame;			//全フレーム数
	};
	struct RecongTask
	{
		const CallbackDataStruct * callback;
		std::string str;

		RecongTask(const CallbackDataStruct * callback,std::string str)
			: callback(callback),str(str)
		{

		}
	};
	std::list<RecongTask> AllCommandRecongTask;
	std::list<RecongTask> AllTemporaryRecongTask;

public:
	Recognition_JuliusPlus();
	virtual ~Recognition_JuliusPlus();

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

private:
	//音声認識ルールを構築します。 正規表現にも対応しています。
	bool AddRegexp(const CallbackDataStruct* callback,const std::string & str ,Recognition_JuliusPlusRule* stateHandle ) ;
	//音声認識ルールを登録する部分の詳細な実行です。正規表現のネストがあるので再起してます。
	bool AddRegexpImpl(const CallbackDataStruct* callback,const std::string & str, Recognition_JuliusPlusRule*  stateHandle);

	//juliusを稼働させます
	void JuliusStop();
	//juliusを停止させます
	bool JuliusStart();
	//juliusのwaveファイルから認識バージョンを起動します。
	bool JuliusFileStart();

	//juliusに食べさせる dfa と dict を生成します
	bool MakeJuliusRule(Recognition_JuliusPlusRule* toprule,bool isNest , bool isInsertGomiNode ,std::ostream* dfa , std::ostream* dict  );

	//結果リストの開放
	void FreeAllSentenceList(std::list<OncSentence*> *allSentence) const;
	//plusのスコアを計算します。
	float computePlusScore(float cmscoreAvg,float score,int sentnum,float mseclen) const;
	//recog から plus 形式に変換します。
	//メモリを余計に消費しますが、こちらのほうが見やすいと思います。
	void convertResult(const Recog *recog, std::list<OncSentence*> *allSentence) const;
	//認識結果の一部を waveで保存する.
	void WaveCutter(Recog *recog,int all_frame,int startfream,int endfream,const std::string& filename) const;
	//ディクテーションフィルターで呼びかけ文字列が入っているか確認する.
	bool checkYobikake(const std::string & dictationString) const;
	//呼びかけがあれば、waveで保存し、もう一つのjuliusインスタンスで再チェックする.
	bool checkDictation(Recog *recog,const OncSentence* sentence) const;
	//仮説の数によるペナルティ
	//仮説が多いと、それだけ判断に困っているということでペナルティを与えます。
	//また、ゴミにマッチした場合はより大きなペナルティを与えます。
	//しきいちは適当です。機械学習とか入れられればいいんだけど。
	int countHypothesisPenalty(const Recog *recog) const;
	//juliusの認識結果と、plusルールに指定されたコールバックを見比べて、
	//正しいであろう、コールバック構造体を返します。
	const CallbackDataStruct* SearchCallbackWhereDICT(const OncSentence* Recognition_JuliusPlusResult) const;
	//正規表現 capture を作ります。
	const std::map<std::string,std::string> CreateCaptureStringWhereDICT(const OncSentence* Recognition_JuliusPlusResult) const;
	std::string ConvertYomi(const WORD_INFO * wordinfo,int index) const;
	//recog から plus 形式に変換します。
	//メモリを余計に消費しますが、こちらのほうが見やすいと思います。
	void convertResultFile(const Recog *recog, std::list<OncSentence*> *allSentence) const;

	//julius のコールバックイベント
	void OnStatusRecready(Recog *recog);
	void OnOutputResult(Recog *recog);
	void OnRecordAdinTrigger(Recog *recog, SP16 *speech, int samplenum);

	void OnOutputResultFile(Recog *recog);

	int LocalCaptureRuleNodeCount;

	double TemporaryRuleConfidenceFilter;
	std::list<std::string> YobikakeListArray;

	Recognition_JuliusPlusRule* TemporaryRuleHandle;
	Recognition_JuliusPlusRule*	GlobalTemporaryRuleHandle;
	Recognition_JuliusPlusRule* YobikakeRuleHandle;
	Recognition_JuliusPlusRule* CommandRuleHandle;
	bool IsNeedUpdateRule;

	std::string DictationCheckString;

	//plus専用の文法保持です。
	Recognition_JuliusPlusRule* Grammer;
	//認識したwaveファイル
	std::vector<SP16> WaveFileData;
	//ダミーのコールバック
	CallbackDataStruct* DummyCallback;
	//Juliusが入力受付状態になっているかのフラグ
	bool JuliusInputReady;

	//マイクから読み込んで実行するjulius
	Jconf *jconf;
	Recog *recog;
	//ディクテーションフィルターのためファイルからのjulius
	//正確には、ディクテーションではなくて、たくさんのゴミの中からの再チェック.
	Jconf *jconfFile;
	Recog *recogFile;

	//juliusをスレッドで動作させます。
	boost::thread *Thread;

	MainWindow* PoolMainWindow;
	//SVM識別器
	XLMachineLearningLibliear SVM;
	//音声データを保存する場所
	std::string LogDirectory;

};
