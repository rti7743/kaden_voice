#include "common.h"
#include <string>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <string>
#include "ScriptRunner.h"
#include "Recognition_Factory.h"
#include "Recognition_JuliusPlus.h"
#include "MainWindow.h"
#include "XLFileUtil.h"

#define issjiskanji(c)	((0x81 <= (unsigned char)(c&0xff) && (unsigned char)(c&0xff) <= 0x9f) || (0xe0 <= (unsigned char)(c&0xff) && (unsigned char)(c&0xff) <= 0xfc))

void Recognition_JuliusPlus::OnStatusRecready(Recog *recog)
{
	//Juliusが入力待ちになっているかどうか
	this->JuliusInputReady = true;
	//認識したwaveファイルを消去します。
	this->WaveFileData.clear();
}

//認識結果の一部を waveで保存する.
void Recognition_JuliusPlus::WaveCutter(Recog *recog,int all_frame,int startfream,int endfream,const std::string& filename) const
{
	if (startfream == 0 && endfream == 0)
	{//all
		endfream = this->WaveFileData.size();
	}
	else
	{
		if (endfream < startfream)
		{
			return ;
		}
	}
	int allsize = this->WaveFileData.size();
	int of_fream = allsize / all_frame;
	startfream *= of_fream;
	endfream *= of_fream;
	//recog->jconf->input.framesize;

	FILE *recfile_fp = wrwav_open( (char*) filename.c_str(), recog->jconf->input.sfreq);
	if(recfile_fp)
	{
		wrwav_data(recfile_fp,(SP16*) ((&this->WaveFileData[0]) + startfream), endfream - startfream);
		wrwav_close(recfile_fp);
	}
}

//ディクテーションフィルターで呼びかけ文字列が入っているか確認する.
bool Recognition_JuliusPlus::checkYobikake(const std::string & dictationString) const
{
	//std::cout << "#checkYobikake " << dictationString << std::endl;
	auto it = this->YobikakeListArray.begin();
	for( ; this->YobikakeListArray.end() != it ; ++it)
	{
		//std::cout << "#checkYobikake :" << dictationString << "VS" << *it << std::endl;
		//ディクテーションフィルターで絞る
		if ( dictationString.find( *it ) != std::string::npos )
		{
			break;
		}
	}
	return (this->YobikakeListArray.end() != it);
}

#include "XLStringUtil.h"

//呼びかけがあれば、waveで保存し、もう一つのjuliusインスタンスで再チェックする.
bool Recognition_JuliusPlus::checkDictation(Recog *recog,const OncSentence* sentence) const
{
	auto it3 = sentence->nodes.begin();
	if (it3 == sentence->nodes.end() )
	{
		return false;
	}
	if (! checkYobikake( (*it3)->wordnode->yomiPress ) )
	{
		return false;
	}
	const std::string waveFilename = XLStringUtil::pathcombine(this->LogDirectory, "__temp__DictationCheck.wav");

	//認識した結果の最初の呼びかけの部分を保存.
	WaveCutter(recog,sentence->all_frame,(*it3)->begin_frame,(*it3)->end_frame, waveFilename );


	//もう一つの julius インスタンスで wave ファイルからの認識をやります.
	//これで過剰マッチを完全に叩き落とします。
	//結果は、this->DictationCheckString に保存されます。
	//char*にしているのはjuliusがC関数なので constがないため・・・
	int ret = j_open_stream(this->recogFile,(char*) waveFilename.c_str() );
	if(ret < 0)
	{
		return false;
	}
	j_recognize_stream(this->recogFile);

	if ( this->DictationCheckString.empty() )
	{//SVMで棄却
		XLFileUtil::move(waveFilename , waveFilename + num2str(time(NULL)) + "_PP.wav" );
//		XLFileUtil::del(waveFilename);
		return false;
	}

	//内容のチェック
	if (! checkYobikake( this->DictationCheckString ) )
	{//SVMはすり抜けたが、ディクテーションフィルタで棄却
		XLFileUtil::move(waveFilename , waveFilename + num2str(time(NULL)) + "_D.wav" );
		return false;
	}
	XLFileUtil::move(waveFilename , waveFilename + num2str(time(NULL)) + "_X.wav" );
	

	return true;
}


void Recognition_JuliusPlus::OnOutputResult(Recog *recog)
{
	//juliusの結果を plus 整形済みパータンとして再構築する.
	std::list<OncSentence*> allSentence;
	convertResult(recog, &allSentence);

	//認識した結果を出す.
	std::cout << std::endl;

	//何も入っていない、ゴミしかないようだったら何もしないでおしまい。
	if (allSentence.empty())
	{
		FreeAllSentenceList(&allSentence);
		return ;
	}
	this->PoolMainWindow->SyncInvokeLog("" ,LOG_LEVEL_DEBUG);
	this->PoolMainWindow->SyncInvokeLog("==---認識結果---------------------------------------------------===" ,LOG_LEVEL_DEBUG);

	int i = 1;
	for(auto it = allSentence.begin() ; it != allSentence.end() ; ++it, ++i)
	{
		std::stringstream word;
		std::stringstream yomi;
		std::stringstream dictNumber;
		std::stringstream cmscore;
		std::stringstream avgscore;
		std::stringstream begin_frame;
		std::stringstream end_frame;
		std::stringstream plus_node_score;

		auto it2 = (*it)->nodes.begin();
		int firstdict = (*it2)->dictNumber;
		for(  ; it2 != (*it)->nodes.end() ; ++it2)
		{
			word << (*it2)->wordnode->word << " ";
			yomi << (*it2)->wordnode->yomi << " ";
			dictNumber << (*it2)->dictNumber << " ";
			cmscore << (*it2)->cmscore << " ";
			avgscore << (*it2)->avgscore << " ";
			begin_frame << (*it2)->begin_frame << " ";
			end_frame << (*it2)->end_frame << " ";
			plus_node_score << (*it2)->plus_node_score << " ";
		}

		

		bool isok = false;
		if ( firstdict != 3 )
		{
			if ( ! ( (*it)->plus_sentence_score >= this->TemporaryRuleConfidenceFilter ) )
			{//テンポラリルールのスコア
				this->PoolMainWindow->SyncInvokeLog(std::string() + " 誤認識、テンポラリルールとしてスコア評価"+num2str((*it)->plus_sentence_score)+"で不適当。認識単語:" + this->DictationCheckString,LOG_LEVEL_DEBUG);
			}
			else
			{
				this->PoolMainWindow->SyncInvokeLog(std::string() + " マッチ。テンポラリルールとしてスコア評価"+num2str((*it)->plus_sentence_score)+"。認識単語:" + this->DictationCheckString,LOG_LEVEL_DEBUG);
				isok = true;
			}
		}
		else
		{
			//検出した呼びかけをもう一度再検証する。
			bool dictationCheck = checkDictation(recog,*it);
			if (!dictationCheck)
			{
				this->PoolMainWindow->SyncInvokeLog(std::string() + " 誤認識、ディクテーションフィルターもどき、呼びかけチェックの結果エラー。認識単語:" + this->DictationCheckString,LOG_LEVEL_DEBUG);
			}
			else
			{
				this->PoolMainWindow->SyncInvokeLog(std::string() + " マッチ!!!!!!!!!!!!!",LOG_LEVEL_DEBUG);
				isok = true;
			}
		}
		this->PoolMainWindow->SyncInvokeLog(std::string() + " word       :" + word.str(),LOG_LEVEL_DEBUG);
		this->PoolMainWindow->SyncInvokeLog(std::string() + " yomi       :" + yomi.str(),LOG_LEVEL_DEBUG);
		this->PoolMainWindow->SyncInvokeLog(std::string() + " dictNumber :" + dictNumber.str(),LOG_LEVEL_DEBUG);
		this->PoolMainWindow->SyncInvokeLog(std::string() + " cmsscore   :" + cmscore.str(),LOG_LEVEL_DEBUG);
		this->PoolMainWindow->SyncInvokeLog(std::string() + " avgscore   :" + avgscore.str(),LOG_LEVEL_DEBUG);
		this->PoolMainWindow->SyncInvokeLog(std::string() + " begin_frame:" + begin_frame.str(),LOG_LEVEL_DEBUG);
		this->PoolMainWindow->SyncInvokeLog(std::string() + " end_frame  :" + end_frame.str(),LOG_LEVEL_DEBUG);
		this->PoolMainWindow->SyncInvokeLog(std::string() + " plus_node_s:" + plus_node_score.str(),LOG_LEVEL_DEBUG);

		if ( isok )
		{//マッチしているのならコールバックを読んであげよう。
			if ( (*it)->nodes.size() <= 1 )
			{//呼びかけ + コマンドと2つのものを見るので、それ以下の物には興味ない
				break;
			}

			const CallbackDataStruct* callback = SearchCallbackWhereDICT(*it);
			if (callback == NULL)
			{
				std::cout << " 有効なコールバックではありません" << std::endl;
			}
			else
			{
				const std::map<std::string,std::string> capture = CreateCaptureStringWhereDICT(*it);
				const double plus_sentence_score = (*it)->plus_sentence_score;

				//マッチしたのでコールバックする
				std::string matchstring=word.str();
				this->PoolMainWindow->SyncInvokePopupMessage("音声認識",matchstring);

				this->PoolMainWindow->AsyncInvoke( [=](){
					//非同期なのでコピーして無効になるようなポインタは使わないでね。
					this->PoolMainWindow->ScriptManager.VoiceRecogntion
						(callback,capture,matchstring,0,plus_sentence_score);
				} );
			}
		}

		//結果は1つだけば十分だろう・・・・
		if (i >= 1)
		{
			break;
		}
	}
	std::cout << std::flush;

	FreeAllSentenceList(&allSentence);
}

//juliusの認識結果と、plusルールに指定されたコールバックを見比べて、
//正しいであろう、コールバック構造体を返します。
const CallbackDataStruct* Recognition_JuliusPlus::SearchCallbackWhereDICT(const OncSentence* juliusPlusResult) const
{
	//呼びかけ + コマンドと2つのものを見るので、それ以下の物には興味ない
	assert( juliusPlusResult->nodes.size() >= 2 );

	auto resultIT = juliusPlusResult->nodes.begin();
	resultIT ++; //最初は呼びかけなので無視する。

	return (*resultIT)->rule->getCallback();
}

//正規表現 capture を作ります。
const std::map<std::string,std::string> Recognition_JuliusPlus::CreateCaptureStringWhereDICT(const OncSentence* juliusPlusResult) const
{
	//呼びかけ + コマンドと2つのものを見るので、それ以下の物には興味ない
	assert( juliusPlusResult->nodes.size() >= 2 );
	
	auto end = juliusPlusResult->nodes.end();
	auto resultIT = juliusPlusResult->nodes.begin();
	resultIT ++; //最初は呼びかけなので無視する。

	std::map<std::string,std::string> ret;
	//最初の一個目は呼びかけ以外の全体を結合したものです。
	{
		for(auto tempIT = resultIT ; tempIT != end ; ++ tempIT)
		{
			ret["0"] += (*tempIT)->wordnode->word;
		}
	}
	//capture を見つけて結合させていきます。
	{
		for(auto captureIT = resultIT ; captureIT != end ; ++ captureIT)
		{
			const int capture = (*captureIT)->rule->getCaptureNumber();
			if (capture == 0)
			{//正規表現なし.
				continue;
			}
			//キャプチャされる子ノードを巡回します。
			//(aaa) だけならいいが、 (aaa(bbb|ccc)) など、子ノードを含む場合がある。
			const std::string captureString = num2str(capture);
			ret[captureString] = (*captureIT)->wordnode->word;

			auto nextIT = captureIT;
			nextIT++;
			for( ; nextIT != end ; ++ nextIT)
			{
				if ( (*captureIT)->rule->IsMemberNode( (*nextIT)->rule ) )
				{
					ret[captureString] += (*nextIT)->wordnode->word;
				}
			}
		}
	}
	return ret;
}



//認識waveファイルの断片が入ります。
void Recognition_JuliusPlus::OnRecordAdinTrigger(Recog *recog, SP16 *speech, int samplenum)
{
	//Waveの断片をつなぎあわせます。
	this->WaveFileData.insert(WaveFileData.end() , speech , speech + samplenum);
	//Julius入力中
	this->JuliusInputReady = false;
}

//結果リストの開放
void Recognition_JuliusPlus::FreeAllSentenceList(std::list<OncSentence*> *allSentence) const
{
	for( auto it = allSentence->begin() ; it != allSentence->end() ; ++it)
	{
		for( auto it2 = (*it)->nodes.begin() ; it2 != (*it)->nodes.end() ; ++it2)
		{
			delete *it2;
		}
		delete *it;
	}
	allSentence->clear();
}

//plusのスコアを計算します。
float Recognition_JuliusPlus::computePlusScore(float cmscoreAvg,float score,int sentnum,float mseclen) const
{
	//まずは、文のスコアを録音時間で割ります。
	float plus_sentence_score = score / mseclen;

	//次に、仮説数に応じてペナルティを与えます。
	//仮説が多いということは、似たようなものが多くて迷っているわけで、これを反映する。
	switch(sentnum)
	{
	case 0:
	case 1:
		//ペナルティなし
		break;
	case 2:
		plus_sentence_score *= 1.1f;
		break;
	case 3:
		plus_sentence_score *= 1.15f;
		break;
	case 4:
		plus_sentence_score *= 1.5f;
		break;
	case 5:
		plus_sentence_score *= 2.0f;
		break;
	default:
		plus_sentence_score *= 3.0f;
		break;
	}

	//cmscore の平均値を反転させて10倍してペナルティとする。大きいほど罰則がでかい
	float cmsplus = (1 - cmscoreAvg + 1) * 10; 
	plus_sentence_score *= cmsplus;	//cmscore によるペナルティを入れる.

	//で、このスコアで見れば、
	//たいていうまく行くと-20程度、失敗すると-100程度に収束するんで、
	//それを 1 - 0 に丸めると、それっぽく見えるでしょwww
	//
	//正しい単語を言った時に 0.80 ぐらいになるように調整します。
	plus_sentence_score = (110 - (plus_sentence_score * -1)) / 100;
	if (plus_sentence_score >= 1) return 1;
	if (plus_sentence_score <= 0) return 0;

	return plus_sentence_score;
}

//仮説の数によるペナルティ
//仮説が多いと、それだけ判断に困っているということでペナルティを与えます。
//また、ゴミにマッチした場合はより大きなペナルティを与えます。
//しきいちは適当です。機械学習とか入れられればいいんだけど。
int Recognition_JuliusPlus::countHypothesisPenalty(const Recog *recog) const
{
	float gomiCount = 0;
	for(RecogProcess* r = recog->process_list; r ; r=r->next) 
	{
		//ゴミを消します。
		if (! r->live || r->result.status < 0 ) 
		{
			continue;
		}
		//同一文章かどうかチェックする
		std::list<int> checkDict;

		auto winfo = r->lm->winfo;
		for(auto n = 0; n < r->result.sentnum; n++) 
		{ // for all sentences
			if (r->result.sent == NULL)  continue;
			const auto s = &(r->result.sent[n]);
			if (s == NULL) continue;
			const auto seq = s->word;
			const auto seqnum = s->word_num;

			//開始が取れないものはごみ
			if ( strcmp(winfo->woutput[seq[0]]  , "<s>") != 0 )
			{
				gomiCount +=1.0f;
				continue;
			}
			if (seqnum <= 2)
			{//start word end 
				gomiCount +=1.0f;
				continue;
			}
			//ゴミ文章ならペナルティ
			if ( strcmp(winfo->woutput[seq[0]]  , "gomi") == 0 )
			{
				gomiCount +=1.0f;
				continue;
			}
			//ふつーのコマンドは スタート符号 呼びかけ コマンド スタート符号終わり の4以上になるよ。
			//コマンドに エアコン(つけて|けして) のように、正規表現が入ると コマンド1 コマンド2 と分割される。
			int dict;
			if ( seqnum >= 4  )
			{
				//同一文章ならペナルティは少なくする  エアコン(つけて|けして) など同一文章
				dict = atoi(winfo->wname[seq[2]]);
			}
			else
			{
				//デモでは使わないがテンポラリルールの場合には、呼びかけが無いのでその分短くなる。
				dict = atoi(winfo->wname[seq[1]]);
			}
			if (dict <= 3)
			{//ゴミルール
				gomiCount +=1.0f;
				continue;
			}

			//すでに登場したノードかどうかしらべる
			if (std::find(checkDict.begin(),checkDict.end() , dict) != checkDict.end()	)
			{
				gomiCount += 0.2f;
				continue;
			}

			//新規の文章なら多少のペナルティ
			gomiCount += 0.5f;
			checkDict.push_back(dict);
		}
	}
	return (int)(gomiCount + 1.5f);
}

//読みを取得する
std::string Recognition_JuliusPlus::ConvertYomi(const WORD_INFO * wordinfo,int index) const
{
	std::string yomi;

	char buf[64];
	for (int j=0;j<wordinfo->wlen[index];j++) 
	{
		yomi += center_name( (wordinfo->wseq[index][j]->name),buf);
	}
	return yomi;
}

//recog から plus 形式に変換します。
//メモリを余計に消費しますが、こちらのほうが見やすいと思います。
void Recognition_JuliusPlus::convertResult(const Recog *recog, std::list<OncSentence*> *allSentence) const
{
	//喋った時間の総数
	const float mseclen = (float)recog->mfcclist->param->samplenum * (float)recog->jconf->input.period * (float)recog->jconf->input.frameshift / 10000.0f;
	//仮説の数によるペナルティ
	const int hypothesisPenalty = this->countHypothesisPenalty(recog);

	for(const RecogProcess* r = recog->process_list; r ; r=r->next) 
	{
		//ゴミを消します。
		if (! r->live || r->result.status < 0 ) 
		{
			continue;
		}

		// output results for all the obtained sentences
		const auto winfo = r->lm->winfo;
		for(auto n = 0; n < r->result.sentnum; n++) 
		{ // for all sentences
			if (r->result.sent == NULL)  continue;
			const auto s = &(r->result.sent[n]);
			if (s == NULL) continue;
			const auto seq = s->word;
			const auto seqnum = s->word_num;

			//開始が取れないものは破棄.
			if ( strcmp(winfo->woutput[seq[0]]  , "<s>") != 0 )
			{
				continue;
			}

			//一文
			OncSentence * oneSentence = new OncSentence;
			//cmsscoreの平均値
			float cmscoreAvg = 0;

			// output word sequence like Julius 
			//0 , [1 , 2, 3, 4], 5, と先頭と最後を除いている、開始終端、記号を抜くため
			for(int i=1;i<seqnum-1;i++)
			{
				//1単語 --> 単語の集合が文になるよ
				int i_seq = seq[i];

				//dict から plus側のrule を求める
				int dict = atoi(winfo->wname[i_seq]);
				const Recognition_JuliusPlusRule* rule = this->Grammer->findDict(dict);
				if (rule == NULL)
				{//plus側にないマッチは無視する
					continue;
				}
				//マッチよみがなを取得する
				std::string yomi = ConvertYomi(winfo,i_seq);
				//よみがなから plus 単語を引く //julius は UTF-8を扱えないので、扱える plus のメモリを参照する形で逃げるw
				const Recognition_JuliusPlusRule::wordnode* wordnode = rule->findYomi(yomi);
				if (wordnode == NULL)
				{//plus側にない単語は無視する
					continue;
				}

				//今の 1文 に1単語を追加.
				auto oneNode = new OneNode();
				oneSentence->nodes.push_back(oneNode);
//				winfo->woutput[i_seq];	//julius は UTF-8を扱えないので plus で登録すると必ず acsiiにする。
														//そのためこの値は yomi とおなじになり、利用できない。
				oneNode->rule = rule;
				oneNode->wordnode = wordnode;
				oneNode->dictNumber = dict;

				//cmscoreの数字(このままでは使えない子状態)
				oneNode->cmscore = s->confidence[i];
				//平均を求めるために追加します。
				cmscoreAvg+=oneNode->cmscore;

				//開始フレームとスコアを取得します。
				for (auto align = s->align; align; align = align->next)
				{
					if (align->unittype != PER_WORD)
					{//-walign 以外無視
						continue;
					}
					for(int ii=0;ii<align->num;ii++) 
					{
						if (align->w[ii] != i_seq)
						{//現在単語ノードi と同じものじゃないと見ない
							continue;
						}

						//開始フレーム
						oneNode->begin_frame = align->begin_frame[ii];
						//終了フレーム
						oneNode->end_frame = align->end_frame[ii];
						//単語のスコア
						oneNode->avgscore = align->avgscore[ii];
						//単語のスコアの尤度
						oneNode->plus_node_score = align->avgscore[ii] / (align->end_frame[ii] - align->begin_frame[ii]);
					}
				}
			}

			if (oneSentence->nodes.empty())
			{//ノードに何も入っていないゴミだったら加えても意味が無いので除去する.
				delete oneSentence;
				continue;
			}
			//julius のスコア 尤度らしい。マイナス値。0に近いほど正しいらしい。
			//「が」、へんてこな文章を入れても、スコアが高くなってしまうし、長い文章を入れるとスコアが絶望的に低くなる
			//ので、そのままだと使えない。
			oneSentence->score = s->score;
			oneSentence->all_frame = r->result.num_frame;

			//cmscore の平均値
			cmscoreAvg = cmscoreAvg / oneSentence->nodes.size();

			//多少使えるスコアを計算します。
			oneSentence->plus_sentence_score = computePlusScore(cmscoreAvg,s->score,hypothesisPenalty,mseclen);
			allSentence->push_back(oneSentence);
		}
	}
}


Recognition_JuliusPlus::Recognition_JuliusPlus()
{
	this->jconf = NULL;
	this->recog = NULL;
	this->Thread = NULL;
	this->recogFile = NULL;
	this->jconfFile = NULL;
	this->IsNeedUpdateRule = true;
	this->LocalCaptureRuleNodeCount = 0;
	this->JuliusInputReady = true; //すぎに終了させてしまうシーンもあるのでとりあえず Ready ということにしておく。
	this->LogDirectory = "";

	this->Grammer = NULL;
}

Recognition_JuliusPlus::~Recognition_JuliusPlus()
{
	JuliusStop();

	delete this->Grammer;
	this->Grammer = NULL;
}

bool Recognition_JuliusPlus::MakeJuliusRule(Recognition_JuliusPlusRule* toprule,bool isNest , bool isInsertGomiNode ,std::ostream* dfa , std::ostream* dict  )
{
	struct reverse_automaton
	{
		//出力する dfaハンドル
		std::ostream* dfa;
		//出力する dictハンドル
		std::ostream* dict;
		//callbackとdfaマッチングテーブル
		//		std::map<int,const CallbackDataStruct*>* callbackMatch;

		//次の dfa 番号
		int dfaNumber ;
		//次の dict 番号
		int dictNumber ;

		//ゴミルールのdict
		int dictGomiNumer ;
		Recognition_JuliusPlus* _this;

		bool isInsertGomiNode;


		void fire(Recognition_JuliusPlusRule* toprule,bool isNest)
		{
			this->dfaNumber = 3;
			this->dictNumber = 3;
			this->dictGomiNumer = 2;

			int firstRuleNumber = this->dfaNumber++; //3
			assert(firstRuleNumber == 3); //0 silE
			//1 silB
			//2 -1 -1
			//なので4からスタート

			*dfa << 0 << " " << 0 << " " << firstRuleNumber << " " << 0 << " " << 1 << std::endl;
			*dict << 0 << "\t" << "[</s>]" << "\t" << " " << "silE" << std::endl;

			*dfa << 1 << " " << 1 << " " << 2 << " " << 1 << " " << 1 << std::endl;
			*dict << 1 << "\t" << "[<s>]" << "\t" << " " << "silB" << std::endl;

			*dfa << 2 << " " << -1 << " " << -1 << " " << 1 << " " << 0 << std::endl;

			if ( this->isInsertGomiNode )
			{
				//ゴミを入れることで juliusの確率計算がかたよるバグを回避する.
				//あ-ん までの一文字のごみを作成します。
				*dfa << firstRuleNumber << " " << this->dictGomiNumer << " " << 1 << " " << 0 << " " << 0 << std::endl;

				//SJISなどではいいんだけどUTFが絡むと問題は深刻なので、愚直にテーブルを作るよ
				const char* akasatanaMapping[] = {"あ" , "い" , "う" , "え" , "お" ,"か" , "き" , "く" , "け" , "こ" ,"さ" , "し" , "す" , "せ" , "そ" ,"た" , "ち" , "つ" , "て" , "と" 
					,"な" , "に" , "ぬ" , "ね" , "の" ,"は" , "ひ" , "ふ" , "へ" , "ほ" ,"ま" , "み" , "む" , "め" , "も" ,"や" , "ゆ" , "よ" 
					,"ら" , "り" , "る" , "れ" , "ろ" ,"わ" , "ん","が","ぎ","ぐ","げ","ご","ざ","じ","ず","ぜ","ぞ","だ","ぢ","づ","で","ど","ば","び","ぶ","べ","ぼ","ぱ","ぴ","ぷ","ぺ","ぽ" };
				for(int akasataI = 0 ; akasataI <  sizeof(akasatanaMapping)/sizeof(akasatanaMapping[0]) ; ++akasataI )
				{
					*dict << this->dictGomiNumer	<< "\t" << "[gomi]" << "\t" << Recognition_JuliusPlusRule::convertYomi(akasatanaMapping[akasataI]) << std::endl;
					*dict << this->dictGomiNumer	<< "\t" << "[gomi]" << "\t" << Recognition_JuliusPlusRule::convertYomi(std::string("こ")+akasatanaMapping[akasataI]) << std::endl;
				}
			}
			if (isNest)
			{//通常のネストするルーチン
				func(toprule,1,0 , firstRuleNumber);
			}
			else
			{//ネストをしないルーチン 単語だけ処理
				nonest_func(toprule,1,0 , firstRuleNumber);
			}
		}

		//rule   処理するルール
		//number ルールに割り振られるべき番号 dfaNumber 番号。
		//backNumber このルールの親ルールの番号
		//termNumber このルールがどこにも遷移しない終端ルールだった時に進む番号
		void func(Recognition_JuliusPlusRule* currentrule, int number,int backNumber,int termNumber)
		{
			assert(currentrule);
			auto words = currentrule->getWords();
			auto nestrules = currentrule->getNestRules();
			Recognition_JuliusPlusRule* termRule = currentrule->getTermRule();

			if (words->size() <= 0 )
			{//単語を伴わないノードだったら
				if (termRule == NULL)
				{//ネストだけのノードだったら自分をスキップする。
					for( auto it = nestrules->begin(); it != nestrules->end() ; ++it )
					{
						func( *it , number ,backNumber , termNumber);
					}
				}
				else
				{//term ruleがあるのでそっちを実効する.
					func( termRule , number ,backNumber,termNumber);

					int nextNumber = this->dfaNumber ++ ;
					for( auto it = nestrules->begin(); it != nestrules->end() ; ++it )
					{
						func( *it , nextNumber ,number , termNumber);
					}
				}
				//スキップするのでココでおわり.
				return ;
			}

			//単語辞書 dictを構築する。
			int dictNumber = this->dictNumber++;
			{
				for(auto it = words->begin(); it != words->end() ; ++it )
				{
					if ((*it)->word == "")
					{
						*dict << dictNumber	<< "\t" << "[<sp>]" << "\t " << "sp" << std::endl;
					}
					else
					{
						//julius が、 EUC-JP 縛りなので、マルチバイトは使わない。
						//						*dict << dictNumber << "\t" << "[" << (*it)->word << "]" << "\t" << " " << (*it)->yomi << std::endl;
						*dict << dictNumber << "\t" << "[_" << (*it)->yomi << "_]" << "\t" << " " << (*it)->yomi << std::endl;
					}
				}
				currentrule->UpdateDictNumber(dictNumber);
			}

			//term ruleがあれば先に実行する。
			if (termRule == NULL)
			{//終端ノードなし
				if (nestrules->size() <= 0 && words->size() >= 1)
				{//単語だけのノードの場合、終端になる。
					*dfa << termNumber << " " << dictNumber << " " << number << " " << 0 << " " << 0 << std::endl;
					if ( this->isInsertGomiNode )
					{
						if (currentrule == _this->YobikakeRuleHandle)
						{
							*dfa << termNumber << " " << this->dictGomiNumer << " " << number << " " << 0 << " " << 0 << std::endl;
						}
					}
				}
				else
				{//単語とネストが共にある場合。
					//ネストの処理を行う。
					int nextNumber = this->dfaNumber ++ ;
					*dfa << nextNumber << " " << dictNumber << " " << number << " " << 0 << " " << 0 << std::endl;

					if (currentrule == _this->YobikakeRuleHandle)
					{
//						int SPnextNumber = this->dfaNumber ++ ;
//						int SPdictNumber = this->dictNumber++;
//						*dict << SPdictNumber	<< "\t" << "[<sp>]" << "\t " << "sp" << std::endl;
//						*dfa << SPnextNumber << " " << dictNumber << " " << number << " " << 0 << " " << 0 << std::endl;
//						*dfa << nextNumber << " " << SPdictNumber << " " << SPnextNumber << " " << 0 << " " << 0 << std::endl;
					}

					for( auto it = nestrules->begin(); it != nestrules->end() ; ++it )
					{
						func( *it , nextNumber ,number , termNumber);
					}
				}
			}
			else
			{
				int nestTermNumber = this->dfaNumber ++ ;

				if (nestrules->size() <= 0 && words->size() >= 1)
				{//単語だけのノードの場合、終端になる。
					*dfa << nestTermNumber << " " << dictNumber << " " << number << " " << 0 << " " << 0 << std::endl;
				}
				else
				{//単語とネストが共にある場合。
					//ネストの処理を行う。
					int nextNumber = this->dfaNumber ++ ;
					*dfa << nextNumber << " " << dictNumber << " " << number << " " << 0 << " " << 0 << std::endl;
					for( auto it = nestrules->begin(); it != nestrules->end() ; ++it )
					{
						func( *it , nextNumber ,number , nestTermNumber);
					}
				}
				func( termRule , nestTermNumber ,number,termNumber);
			}
		}

		//rule   処理するルール
		//number ルールに割り振られるべき番号 dfaNumber 番号。
		//backNumber このルールの親ルールの番号
		//termNumber このルールがどこにも遷移しない終端ルールだった時に進む番号
		void nonest_func(const Recognition_JuliusPlusRule* currentrule, int number,int backNumber,int termNumber)
		{
			assert(currentrule);
			auto words = currentrule->getWords();
			auto nestrules = currentrule->getNestRules();
			auto termRule = currentrule->getTermRule();

			//単語辞書 dictを構築する。
			int dictNumber = this->dictNumber++;
			{
				for(auto it = words->begin(); it != words->end() ; ++it )
				{
					if ((*it)->word == "")
					{
						*dict << dictNumber	<< "\t" << "[<sp>]" << "\t " << "sp" << std::endl;
					}
					else
					{
						*dict << dictNumber << "\t" << "[" << (*it)->word << "]" << "\t" << " " << (*it)->yomi << std::endl;
					}
				}
			}

			*dfa << termNumber << " " << dictNumber << " " << number << " " << 0 << " " << 0 << std::endl;
			if (currentrule == _this->YobikakeRuleHandle)
			{
				if ( this->isInsertGomiNode )
				{
					if (currentrule == _this->YobikakeRuleHandle)
					{
						*dfa << termNumber << " " << this->dictGomiNumer << " " << number << " " << 0 << " " << 0 << std::endl;
					}
				}
			}
		}
	} maton;
	maton._this = this;
	maton.dfa = dfa;
	maton.dict = dict;
	maton.isInsertGomiNode = isInsertGomiNode;
	maton.fire( toprule ,isNest);

	return true;
}

//音声認識ルールを構築します。 正規表現にも対応しています。
xreturn::r<bool> Recognition_JuliusPlus::AddRegexp(const CallbackDataStruct* callback,const std::string & str ,Recognition_JuliusPlusRule* stateHandle ) 
{
	//unixへの移植を考えて wchar_tはやめることにした。
	//unix で UTF8 を使っている限りはマルチバイトに悩むことはないでしょう。 EUCとかレガシーなシステムは知らん。
	//	_USE_WINDOWS_ENCODING;

	//一番最初だけ正規表現の構文変換をかける.
	//難しすぎる表現を簡単なゆとり仕様に直して、パースしやすくする。
	// .+ --> (:?.*)
	// (まる|さんかく)? --> (まる|さんかく|)   正しい正規表現としてはエラーだが、このエンジンの場合容認する.
	// なのは? --> なの(は|)
	std::string optstr;
	optstr.reserve(str.size());

	for( const char * p = str.c_str()  ; *p ; ++p )
	{
#ifdef _WINDOWS_
		if ( issjiskanji(*p) )
		{
			optstr += *p;
			optstr += *(p+1);
			p ++;
			continue;
		}
#endif
		if ( *p == '.' && *(p+1) == '+')
		{ // .+ --> (:?.*)
			optstr += "(?:.+)";
			++p;
		}
		else if (*p == '(' && *(p+1) == '?' && *(p+2) == ':' )
		{
			optstr += "(?:";
			p+=2;
		}
		else if (*(p+1) == '?')
		{
			if (*p == ')')
			{// (まる|さんかく)? --> (まる|さんかく|)
				optstr += "|)";
			}
			else 
			{// なのは? --> なの(は|)
				optstr += std::string("(?:") + *p + "|)";
			}
			++p;
		}
		else if (*p == '*' || *p == '+' || *p == '.' || *p == '[' || *p == ']')
		{
			//            throw exception(std::string("") + "現在は、メタ文字 " + p + " は利用できません。利用可能なメタ文字 () | .+ ?");
			return xreturn::error(std::string("") + "現在は、メタ文字 " + p + " は利用できません。利用可能なメタ文字 () | .+ ?");
		}
		else
		{
			optstr += *p;
		}
	}

	//正規表現 captureの番号。
	this->LocalCaptureRuleNodeCount = 1;
	//このルールを収めるメールを作る。(この処理は厳格に見ると非効率だが、とりあえずこれで)
	Recognition_JuliusPlusRule* firstNestRule = new Recognition_JuliusPlusRule(callback);
	stateHandle->AddNestRule(firstNestRule);
	//正規表現パースを本気でやる
	return AddRegexpImpl(callback,optstr, firstNestRule);
}

//音声認識ルールを登録する部分の詳細な実行です。正規表現のネストがあるので再起してます。
xreturn::r<bool> Recognition_JuliusPlus::AddRegexpImpl(const CallbackDataStruct* callback,const std::string & str, Recognition_JuliusPlusRule*  stateHandle)
{
	//	_USE_WINDOWS_ENCODING;
	std::string matchString;

	//正規表現をパースしながら回す.
	const char * p = str.c_str();
	const char * splitPos = p;
	Recognition_JuliusPlusRule*  currentRule = stateHandle;
	for(  ; *p ; ++p )
	{
#ifdef _WINDOWS_
		if ( issjiskanji(*p) )
		{
			p ++;
			continue;
		}
#endif
		if (*p == '(')
		{
			//閉じ括弧まで飛ばす. )
			int nest = 1;
			const char* n = p + 1;
			for( ; *n  ; ++n )
			{
#ifdef _WINDOWS_
				if ( issjiskanji(*n) )
				{
					n ++;
					continue;
				}
#endif
				if (*n == '(')
				{
					++nest ;
				}
				else if (*n == ')')
				{
					--nest ;
					if (nest <= 0)
					{
						break;
					}
				}
			}

			//ネストする前の部分
			matchString = std::string(splitPos, 0 ,p - splitPos);


			//ネストしている部分を格納するルールを作る.
			Recognition_JuliusPlusRule*  nestRule;

			//お目当ての()ネストを格納する部分を作る
			//キャプチャー？
			if (*(p+1) == '?' && *(p+2) == ':')
			{
				p += 2;
				nestRule = new Recognition_JuliusPlusRule(callback);
			}
			else
			{
				nestRule = new Recognition_JuliusPlusRule( this->LocalCaptureRuleNodeCount++ , callback);
			}

			if (matchString.empty())
			{//先行文字列が空
				currentRule->AddNestRule(nestRule);
			}
			else
			{//文字列だけを入れると OR扱いになってしまうので、ネストを格納するネストを作らないとダメ。
				Recognition_JuliusPlusRule* firstNestRule = new Recognition_JuliusPlusRule(callback);
				firstNestRule->AddWord(matchString);

				firstNestRule->AddNestRule(nestRule);
				currentRule->AddNestRule(firstNestRule);
			}

			//かっこの後にも構文が連続する場合、そのツリーを作成する.
			if (*n == '\0'  || *(n+1) == '\0' || *(n+1) == '|')
			{//閉じかっこで構文がとまる場合はそこで終端
				//nop
			}
			else
			{//さらに構文が続いている場合
				Recognition_JuliusPlusRule * afterRule = new Recognition_JuliusPlusRule(callback);
				nestRule->SetCombineRule(afterRule);

				currentRule = afterRule;
			}

			//ネストしているルールを再帰して実行.
			matchString = std::string(p+1 , 0 , (int) (n - p - 1) );
			auto r = this->AddRegexpImpl(callback,matchString, nestRule);
			if(!r)
			{
				return xreturn::errnoError(r);
			}

			p = n ;
			splitPos = n + 1;  //+1は最後の ) を飛ばす. iは forの ++i で i == splitPos となる。(わかりにくい)
		}
		else if (*p == '|') 
		{
			matchString = std::string(splitPos,0 , (int) (p - splitPos));
			if (matchString.length() >= 1)
			{
				currentRule->AddWord(matchString);
			}
			//空分岐 (A|) のような場合、空ノードを入れる.
			if (matchString.length() <= 0 || *(p+1) == '\0' )
			{
				assert(0); //not implement! 未実装!
				currentRule->AddWord("");
			}

			splitPos = p + 1;
			currentRule = stateHandle;
		}
		else if (*p == '.' && *(p+1) == '+') 
		{
			assert(0); //not implement! 未実装!
			//			currentRule->AddWord("＊");

			p += 1;
			splitPos = p + 1;
		}
	}

	//最後の残り
	matchString = std::string(splitPos , 0 , (int) (p - splitPos) );
	if ( matchString.length() >= 1 &&str.length() >= 1 && *(p-1) != ')')
	{
		currentRule->AddWord(matchString );
	}
	return true;
}



xreturn::r<bool> Recognition_JuliusPlus::Create(MainWindow* poolMainWindow)
{
	assert(this->Grammer == NULL);
	this->PoolMainWindow = poolMainWindow;

	this->DummyCallback = NULL;

	//ルール構文
	//grammertop
	//	AddNestRule -- 
	//                 YobikakeRule
	//                       AddNestRule --- CommandRule
	//  AddNestRule -- TemporaryRule
	//
	this->Grammer = new Recognition_JuliusPlusRule(this->DummyCallback);
	this->YobikakeRuleHandle = new Recognition_JuliusPlusRule(this->DummyCallback);
	this->CommandRuleHandle = new Recognition_JuliusPlusRule(this->DummyCallback);

	this->GlobalTemporaryRuleHandle = new Recognition_JuliusPlusRule(this->DummyCallback);
	this->TemporaryRuleHandle = new Recognition_JuliusPlusRule(this->DummyCallback);

	//全部 Grammer に関連付けます。
	//Grammer が delete されれば、残りもすべてデリートされます。
	this->Grammer->AddNestRule(this->YobikakeRuleHandle);
	this->Grammer->AddNestRule(this->GlobalTemporaryRuleHandle);
	this->Grammer->AddNestRule(this->TemporaryRuleHandle);
	this->YobikakeRuleHandle->AddNestRule(this->CommandRuleHandle);
	return true;
}

//呼びかけを設定します。
//設定したあと、 CommitRule() てしてね。
xreturn::r<bool> Recognition_JuliusPlus::SetYobikake(const std::list<std::string> & yobikakeList)
{
	this->YobikakeListArray.clear();
	for(auto it = yobikakeList.begin();  yobikakeList.end() != it ; ++it)
	{
		this->YobikakeListArray.push_back( Recognition_JuliusPlusRule::convertYomiAndPress(*it) );
		this->YobikakeRuleHandle->AddWord(*it );
	}
	return true;
}

xreturn::r<bool> Recognition_JuliusPlus::SetCancel(const std::list<std::string> & cancelList) 
{
	for(auto it = cancelList.begin();  cancelList.end() != it ; ++it)
	{
//		this->GlobalTemporaryRuleHandle->AddWord(*it );
	}
	return true;
}


//認識結果で不完全なものを捨てる基準値を設定します。
xreturn::r<bool> Recognition_JuliusPlus::SetRecognitionFilter(double temporaryRuleConfidenceFilter)
{
	this->TemporaryRuleConfidenceFilter = temporaryRuleConfidenceFilter;

	return true;
}

//音声データを保存するディレクトリ
xreturn::r<bool> Recognition_JuliusPlus::SetLogDirectory(const std::string& logdir)
{
	this->LogDirectory = logdir;

	return true;
}


//コマンドに反応する音声認識ルールを構築します
xreturn::r<bool> Recognition_JuliusPlus::AddCommandRegexp(const CallbackDataStruct* callback,const std::string & str)
{
	this->IsNeedUpdateRule = true;
	return AddRegexp(callback,str , this->CommandRuleHandle);
}

//テンポラリルールに反応する音声認識ルールを構築します
xreturn::r<bool> Recognition_JuliusPlus::AddTemporaryRegexp(const CallbackDataStruct* callback,const std::string & str)
{
	this->IsNeedUpdateRule = true;
	this->TemporaryRuleCount ++;
	return AddRegexp(callback,str, this->TemporaryRuleHandle);
}

//テンポラリルールをすべてクリアします
xreturn::r<bool> Recognition_JuliusPlus::ClearTemporary()
{
	if (this->TemporaryRuleCount <= 0)
	{//現在テンポラリルールにルールが入っていないので、クリアをスキップします。
		return true;
	}

	//テンポラリールールを白紙に
	this->TemporaryRuleHandle->Clear();

	//クリアしたので、テンポラリルールの数はゼロになります。
	this->TemporaryRuleCount = 0;
	//ルールに変更が加わったのでコミットしないといけません。
	this->IsNeedUpdateRule = true;

	//コミット発動
	auto r = this->CommitRule();
	if (!r) return xreturn::error(r.getError());

	return true;
}

//構築したルールを音声認識エンジンにコミットします。
xreturn::r<bool> Recognition_JuliusPlus::CommitRule()
{
	if (! this->IsNeedUpdateRule )
	{//アップデートする必要なし
		return true;
	}

	//マイクから入力用
	std::ofstream dfa("__temp__regexp_test.dfa");
	std::ofstream dict("__temp__regexp_test.dict");
	this->MakeJuliusRule(this->Grammer,true,true, &dfa,&dict);

	//ディクテーションフィルター用
	std::ofstream dfaFile("__temp__regexp_test_file.dfa");
	std::ofstream dictFile("__temp__regexp_test_file.dict");
	this->MakeJuliusRule(this->YobikakeRuleHandle,false,true,&dfaFile,&dictFile);

	this->JuliusStop();
	this->JuliusFileStart();
	this->JuliusStart();

	//アップデートが終わったので再びルールに変更が加わるまではアップデートしない。
	this->IsNeedUpdateRule = false;

	return true;
}


void Recognition_JuliusPlus::JuliusStop()
{
	if (this->recog)
	{
		//Juliusがビジーでなくなるまで待機
		while(! this->JuliusInputReady )
		{
//			::Sleep(0);
		}

		//ストリームを閉じる
		j_close_stream(this->recog);

		//スレッド停止までまつ
		this->Thread->join();
		delete this->Thread;
		this->Thread = NULL;

		if (this->jconf)
		{
			//これで開放すると、 j_recog_free で落ちる・・・
			//			j_jconf_free(this->jconf);
			this->jconf = NULL;
		}
		//メモリ開放
		j_recog_free(this->recog);
		this->recog = NULL;
	}

	if (this->recogFile)
	{
		if (this->jconfFile)
		{
			//これで開放すると、 j_recog_free で落ちる・・・
			//			j_jconf_free(this->jconfFile);
			this->jconfFile = NULL;
		}

		j_recog_free(this->recogFile);
		this->recogFile = NULL;
	}
}

xreturn::r<bool> Recognition_JuliusPlus::JuliusStart()
{
	assert(this->recog == NULL);
	assert(this->jconf == NULL);
	assert(this->Thread == NULL);
	const char* argv[]={
		"juliusplus"
		,"-C"
		,"testmic.jconf"
	};
	int argc = sizeof(argv)/sizeof(argv[0]);
	int ret;
	//SVM学習モデルをロード
	if (! this->SVM.LoadModel("__svm_model.dat") )
	{
		return xreturn::error("SVMモデルをロードできません");
	}

	//julusはC関数なので、const外して char** にするしかない・・・
	this->jconf = j_config_load_args_new(argc, (char**)argv);
	/* else, you can load configurations from a jconf file */
	//jconf = j_config_load_file_new(jconf_filename);
	if (this->jconf == NULL) 
	{
		return xreturn::error("Try `-help' for more information.\n");
	}

	/* 2. create recognition instance according to the jconf */
	/* it loads models, setup final parameters, build lexicon
	and set up work area for recognition */
	this->recog = j_create_instance_from_jconf(this->jconf);
	if (this->recog == NULL)
	{
		return xreturn::error("Error in startup(j_create_instance_from_jconf)\n");
	}
	struct _ref{
		static void status_recready(Recog *recog, void *_this)
		{
			((Recognition_JuliusPlus*)_this)->OnStatusRecready(recog);
		}
		static void output_result(Recog *recog, void *_this)
		{
			((Recognition_JuliusPlus*)_this)->OnOutputResult(recog);
		}
		static void record_adin_trigger(Recog *recog, SP16 *speech, int samplenum, void *_this)
		{
			((Recognition_JuliusPlus*)_this)->OnRecordAdinTrigger(recog,speech,samplenum);
		}
	};
	callback_add(this->recog, CALLBACK_EVENT_SPEECH_READY, _ref::status_recready, this);
	callback_add(this->recog, CALLBACK_RESULT, _ref::output_result, this);
	callback_add_adin(this->recog, CALLBACK_ADIN_TRIGGERED, _ref::record_adin_trigger, this);

	// Initialize audio input
	if (j_adin_init(this->recog) == FALSE) 
	{
		return xreturn::error("Error in startup(j_adin_init)\n");
	}

	//output system information to log
	//j_recog_info(this->recog);
	ret = j_open_stream(recog, NULL);
	if(ret < 0)
	{
		return xreturn::error("Error in startup(j_open_stream)\n");
	}

	this->Thread = new boost::thread( [&]()
	{
		j_recognize_stream(recog);
	} );
	return true;
}



//ディクテーションフィルター利用時の認識
void Recognition_JuliusPlus::OnOutputResultFile(Recog *recog)
{
	this->DictationCheckString = "";
	//喋った時間の総数
	const float mseclen = (float)recog->mfcclist->param->samplenum * (float)recog->jconf->input.period * (float)recog->jconf->input.frameshift / 10000.0f;
	//仮説の数によるペナルティ
	const int hypothesisPenalty = countHypothesisPenalty(recog);

	for(const RecogProcess* r = recog->process_list; r ; r=r->next) 
	{
		//ゴミを消します。
		if (! r->live || r->result.status < 0 ) 
		{
			continue;
		}

		// output results for all the obtained sentences
		const auto winfo = r->lm->winfo;
		for(auto n = 0; n < r->result.sentnum; n++) 
		{ // for all sentences
			if (r->result.sent == NULL)  continue;
			const auto s = &(r->result.sent[n]);
			if (s == NULL) continue;
			const auto seq = s->word;
			const auto seqnum = s->word_num;

			int i_seq;
			// output word sequence like Julius 
			//0 , [1 , 2, 3, 4], 5, と先頭と最後を除いている、開始終端、記号を抜くため
			int i;
			for(i = 0;i<seqnum;i++)
			{
				//1単語 --> 単語の集合が文になるよ
				i_seq = seq[i];

				//開始と終了を飛ばす
				if (	strcmp(winfo->woutput[i_seq]  , "<s>") == 0 
					||  strcmp(winfo->woutput[i_seq]  , "</s>") == 0 
				){
					continue;
				}
				break;
			}
			if (i >= seqnum) 
			{
				continue;
			}
			//素性を詰めていきます。
			std::vector<XLMachineLearningLibliear::feature> feature_nodeVector;
			feature_nodeVector.resize(r->lm->am->mfcc->param->header.samplenum * r->lm->am->mfcc->param->veclen + 9 + 1);
			auto feature_nodeP = &feature_nodeVector[0];

			//dict から plus側のrule を求める
			const int dict = atoi(winfo->wname[i_seq]);

			//マッチよみがなを取得する
			const std::string yomi = ConvertYomi(winfo,i_seq);

			//cmscoreの数字(このままでは使えない子状態)
			const auto cmscore = s->confidence[i];

			//素性1 cmsscore
			feature_nodeP->index = 1;
			feature_nodeP->value = cmscore;
			feature_nodeP++;

			//julius のスコア 尤度らしい。マイナス値。0に近いほど正しいらしい。
			//「が」、へんてこな文章を入れても、スコアが高くなってしまうし、長い文章を入れるとスコアが絶望的に低くなる
			//ので、そのままだと使えない。
			const auto score = s->score;
			const auto all_frame = r->result.num_frame;

			//素性2 文章スコアをフレームで割ったもの
			feature_nodeP->index = 2;
			feature_nodeP->value = (int)(score/all_frame);
			feature_nodeP++;

			//素性3 フレーム数
			feature_nodeP->index = 3;
			feature_nodeP->value = all_frame;
			feature_nodeP++;

			//素性4 仮説の数によるペナルティ
			feature_nodeP->index = 4;
			feature_nodeP->value = hypothesisPenalty;
			feature_nodeP++;

			//素性5 録音時間
			feature_nodeP->index = 5;
			feature_nodeP->value = mseclen;
			feature_nodeP++;

			//多少使えるスコアを計算します。
			//			oneSentence->plus_sentence_score = computePlusScore(oneSentence->nodes,s->score,r->result.sentnum,mseclen);
			const auto plus_sentence_score = computePlusScore(cmscore,s->score,hypothesisPenalty,mseclen);
			//素性6 plusスコア
			feature_nodeP->index = 6;
			feature_nodeP->value = plus_sentence_score;
			feature_nodeP++;

			//素性7 サンプル数？
			feature_nodeP->index = 7;
			feature_nodeP->value = r->lm->am->mfcc->param->header.samplenum;
			feature_nodeP++;

			//素性8 ゴミから拾い上げれたか？
			feature_nodeP->index = 8;
			feature_nodeP->value = dict * 100.f;
			feature_nodeP++;

			//素性9 サイズ
			feature_nodeP->index = 9;
			feature_nodeP->value = this->WaveFileData.size() / 1024;
			feature_nodeP++;

			//素性10～ これがきめてになった。
			int feature = 10;
			const int samplenumSecond =  min(r->lm->am->mfcc->param->header.samplenum,200);
			for(int vecI = 0 ; vecI < samplenumSecond;vecI++ )
			{
				for(int vecN = 0 ; vecN < r->lm->am->mfcc->param->veclen ;vecN++ )
				{
					feature_nodeP->index = feature;
					feature_nodeP->value = r->lm->am->mfcc->param->parvec[vecI][vecN];
					feature++;
					feature_nodeP++;
				}
			}

			//終端
			feature_nodeP->index = feature;
			feature_nodeP->value = -1;
			
			int classid = this->SVM.Predict(feature_nodeVector);
			if ( classid != 1 )
			{
				std::cout << "SVMによる棄却" << std::endl;
				this->DictationCheckString = "";
				return ;
			}
			//たぶんこれが呼びかけ
			this->DictationCheckString = Recognition_JuliusPlusRule::converPress( ConvertYomi(winfo,i_seq) );
			return ;
		}
	}
}

xreturn::r<bool> Recognition_JuliusPlus::JuliusFileStart()
{
	assert(this->recogFile == NULL);
	assert(this->jconfFile == NULL);
	const char* argv[]={
		"juliusplus"
		,"-C"
		,"testfile.jconf"
	};
	int argc = sizeof(argv)/sizeof(argv[0]);

	//julius は C関数だから、const外して char** にするしかない。
	this->jconfFile = j_config_load_args_new(argc, (char**)argv);
	/* else, you can load configurations from a jconf file */
	//jconf = j_config_load_file_new(jconf_filename);
	if (this->jconfFile == NULL) 
	{
		return xreturn::error("Try `-help' for more information.\n");
	}

	/* 2. create recognition instance according to the jconf */
	/* it loads models, setup final parameters, build lexicon
	and set up work area for recognition */
	this->recogFile = j_create_instance_from_jconf(this->jconfFile);
	if (this->recogFile == NULL)
	{
		return xreturn::error("Error in startup(j_create_instance_from_jconf)\n");
	}


	struct _ref{
		static void output_result(Recog *recog, void *_this)
		{
			((Recognition_JuliusPlus*)_this)->OnOutputResultFile(recog);
		}
	};
	callback_add(this->recogFile, CALLBACK_RESULT, _ref::output_result, this);

	// Initialize audio input
	if (j_adin_init(this->recogFile) == FALSE) 
	{
		return xreturn::error("Error in startup(j_adin_init)\n");
	}
	//以上、準備だけしておいて、
	//認識ルーチンは、後から呼びます。
/*
	int ret = j_open_stream(recogFile, "nano.wav");
	if(ret < 0)
	{
		return xreturn::error("Error in startup(j_open_stream)\n");
	}

	j_recognize_stream(recogFile);

	ret = j_open_stream(recogFile, "nano.wav");
	if(ret < 0)
	{
		return xreturn::error("Error in startup(j_open_stream)\n");
	}

	j_recognize_stream(recogFile);
*/
	return true;
}

//このコールバックに関連付けられているものをすべて消す
xreturn::r<bool> Recognition_JuliusPlus::RemoveCallback(const CallbackDataStruct* callback , bool is_unrefCallback) 
{
	return true;
}
//メディア情報をアップデートします。
xreturn::r<bool> Recognition_JuliusPlus::UpdateMedia(const std::string& name ,const std::list<std::string>& list ) 
{
	return true;
}
