#include "common.h"
#include <string>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <string>
#include "ScriptRunner.h"
#include "Recognition_Factory.h"
#include "Recognition_Cloud.h"
#include "MainWindow.h"
#include "XLFileUtil.h"
#include "XLStringUtil.h"
#include "methodcallback.h"
#include "XLHttpSocket.h"

Recognition_Cloud::Recognition_Cloud()
{
	this->jconf = NULL;
	this->recog = NULL;
	this->Thread = NULL;

	this->IsNeedUpdateRule = true;
	this->LogDirectory = "";

	this->CloudServer = "http://127.0.0.1:11920";
}

Recognition_Cloud::~Recognition_Cloud()
{
	JuliusStop();

	for(auto it = this->RegexpList.begin() ; it != this->RegexpList.end() ; it ++ )
	{
		delete *it;
	}
	this->RegexpList.clear();
	for(auto it = this->RegexpTemporaryList.begin() ; it != this->RegexpTemporaryList.end() ; it ++ )
	{
		delete *it;
	}
	this->RegexpTemporaryList.clear();
}



bool Recognition_Cloud::Create(MainWindow* poolMainWindow)
{
	this->PoolMainWindow = poolMainWindow;

	return true;
}

//呼びかけを設定します。
//設定したあと、 CommitRule() てしてね。
bool Recognition_Cloud::SetYobikake(const std::list<std::string> & yobikakeList)
{
	return true;
}
bool Recognition_Cloud::SetCancel(const std::list<std::string> & cancelList) 
{
	return true;
}

//認識結果で不完全なものを捨てる基準値を設定します。
bool Recognition_Cloud::SetRecognitionFilter(double temporaryRuleConfidenceFilter)
{
	return true;
}

//音声データを保存するディレクトリ
bool Recognition_Cloud::SetLogDirectory(const std::string& logdir)
{
	this->LogDirectory = logdir;

	return true;
}

//コマンドに反応する音声認識ルールを構築します
bool Recognition_Cloud::AddCommandRegexp(const CallbackDataStruct* callback,const std::string & str)
{
	this->IsNeedUpdateRule = true;
	this->RegexpList.push_back( new CallbackRegexpStruct(callback,str) );

	return true;
}

//テンポラリルールに反応する音声認識ルールを構築します
bool Recognition_Cloud::AddTemporaryRegexp(const CallbackDataStruct* callback,const std::string & str)
{
	this->IsNeedUpdateRule = true;
	this->RegexpTemporaryList.push_back( new CallbackRegexpStruct(callback,str) );
	this->TemporaryRuleCount ++;

	return true;
}

//テンポラリルールをすべてクリアします
bool Recognition_Cloud::ClearTemporary()
{
	if (this->TemporaryRuleCount <= 0)
	{//現在テンポラリルールにルールが入っていないので、クリアをスキップします。
		return true;
	}

	//テンポラリールールを白紙に
	for(auto it = this->RegexpTemporaryList.begin() ; it != this->RegexpTemporaryList.end() ; it ++ )
	{
		delete *it;
	}
	this->RegexpTemporaryList.clear();

	//クリアしたので、テンポラリルールの数はゼロになります。
	this->TemporaryRuleCount = 0;
	//ルールに変更が加わったのでコミットしないといけません。
	this->IsNeedUpdateRule = true;

	//コミット発動
	this->CommitRule();

	return true;
}

//構築したルールを音声認識エンジンにコミットします。
bool Recognition_Cloud::CommitRule()
{
	if (! this->IsNeedUpdateRule )
	{//アップデートする必要なし
		return true;
	}
	this->PoolMainWindow->SyncInvokeLog(std::string("") + "link取得中・・・・");
	//linkの取得
	std::string link = XLHttpSocket::Get(this->CloudServer+"/getlink?username=rti&password=hash" );
	this->PoolMainWindow->SyncInvokeLog(std::string("") + "link取得に成功:" + link);

	std::string uploadtext;
	auto i = 1;
	for(auto it = this->RegexpList.begin() ; it != this->RegexpList.end() ; ++it  , ++i )
	{
		uploadtext += num2str( i ) + "\t1\t" +  (*it)->regexp + "\r\n";
	}

	i = 1000001;	//下駄
	for(auto it = this->RegexpTemporaryList.begin() ; it != this->RegexpTemporaryList.end() ; ++it , ++i )
	{
		uploadtext += num2str( i ) + "\t2\t" +  (*it)->regexp + "\r\n";
	}
	this->PoolMainWindow->SyncInvokeLog(std::string("") + "クラウドのルールをアップデートします");

	//クラウドのルールをアップデートする.
	XLHttpSocket::Post(this->CloudServer+"/recogupdate?link=123456789",uploadtext.c_str(),uploadtext.size() );

	this->PoolMainWindow->SyncInvokeLog(std::string("") + "クラウドのルールをアップデートしました");


	//アップデートが終わったので再びルールに変更が加わるまではアップデートしない。
	this->IsNeedUpdateRule = false;

	JuliusStop();	//本当はなくしたい。
	JuliusStart();

	return true;
}

void Recognition_Cloud::JuliusStop()
{
	if (this->recog)
	{
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
}



int Recognition_Cloud::adin_callback_file(SP16 *now, int len, Recog *recog)
{
	/* if input length reaches limit, rehash the ad-in buffer */
	if (len <= 0) return 0;

	if (WaveFileData.size() > MAXSPEECHLEN - 16000) {
		recog->adin->rehash = TRUE;
	}
	
	//バッファリング
	WaveFileData.insert(WaveFileData.end() , now , now + len );
//	this->PoolMainWindow->SyncInvokeLog(std::string("音声") + "a" + num2str(WaveFileData.size() ) + " " + num2str(len) );

	return(0);
}

bool Recognition_Cloud::JuliusStart()
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

	//julusはC関数なので、const外して char** にするしかない・・・
	this->jconf = j_config_load_args_new(argc, (char**)argv);
	/* else, you can load configurations from a jconf file */
	//jconf = j_config_load_file_new(jconf_filename);
	if (this->jconf == NULL) 
	{
		throw XLException("Try `-help' for more information.\n");
	}

	/* 2. create recognition instance according to the jconf */
	/* it loads models, setup final parameters, build lexicon
	and set up work area for recognition */
	this->recog = j_create_instance_from_jconf(this->jconf);
	if (this->recog == NULL)
	{
		throw XLException("Error in startup(j_create_instance_from_jconf)\n");
	}

	// Initialize audio input
	if (j_adin_init(this->recog) == FALSE) 
	{
		throw XLException("Error in startup(j_adin_init)\n");
	}

	//output system information to log
	//j_recog_info(this->recog);
	ret = j_open_stream(this->recog, NULL);
	if(ret < 0)
	{
		throw XLException("Error in startup(j_open_stream)\n");
	}

	this->Thread = new boost::thread( [&]()
	{
		this->ThreadMain();
	} );
	return true;
}

void Recognition_Cloud::ThreadMain()
{
	int ret;
	do
	{
		this->WaveFileData.clear();

		ret = adin_go(
			methodcallback::regist<struct _adin_go_callback,int (*)(SP16 *, int, Recog *) >(this, &Recognition_Cloud::adin_callback_file)
			, NULL
			, this->recog);
		if(this->WaveFileData.empty())
		{
			continue;
		}

		std::string filename="uploadtemp.wav";
		FILE *recfile_fp = wrwav_open( (char*) filename.c_str(), this->recog->jconf->input.sfreq);
		if(recfile_fp)
		{
			wrwav_data(recfile_fp,&this->WaveFileData[0],WaveFileData.size());
			wrwav_close(recfile_fp);
		}
		this->PoolMainWindow->SyncInvokeLog(std::string("音声") + "l" + num2str(WaveFileData.size() ) );

		std::vector<char> vec;
		XLFileUtil::cat_b(filename,&vec);
		std::string retString = XLHttpSocket::Post(this->CloudServer+"/recogwave?link=123456789",&vec[0],vec.size() );

		//認識結果
		this->PoolMainWindow->SyncInvokeLog(std::string("結果") + retString );

		Reaction(retString);
	}while(ret != -1);
}

void Recognition_Cloud::Reaction(const std::string &recogResult)
{
	std::map<std::string,std::string> capture;

	const char* p = recogResult.c_str();
	if (!p) return ;
	const char * pp = strstr(p,"\r\n");
	if (!pp) return ;

	//1行目にIDが入っている
	unsigned int id = atoi(p);

	//2行目に
	for(int i = 0 ; 1  ; ++i )
	{
		p = pp + 2;
		pp = strstr(p,"\r\n");
		if (!pp) break ;

		capture[ num2str(i) ] = std::string( p , 0 ,(int) (pp - p) );
	}
	if ( capture.empty() || capture.at("0") == "" )
	{
		return ;
	}

	//IDからマッチしたものを探す.
	const CallbackDataStruct* callback = NULL;
	if (id >= 1000000)
	{
		int i = 1000001;
		for(auto it = this->RegexpTemporaryList.begin() ; it != this->RegexpTemporaryList.end() ; ++it ,++i )
		{
			if (i == id)
			{
				callback = (*it)->callback;
				break;
			}
		}
	}
	else
	{
		int i = 1;
		for(auto it = this->RegexpList.begin() ; it != this->RegexpList.end() ; ++it  ,++i )
		{
			if (i == id)
			{
				callback = (*it)->callback;
				break;
			}
		}
	}

	if (callback == NULL)
	{//ない!!
		return ;
	}

	//マッチしたのでコールバックする
	const std::string mathString = capture["0"];
	this->PoolMainWindow->SyncInvokePopupMessage("音声認識", mathString);

	this->PoolMainWindow->AsyncInvoke( [=](){
		//非同期なのでコピーして無効になるようなポインタは使わないでね。
		this->PoolMainWindow->ScriptManager.VoiceRecogntion
			(callback,capture,mathString,0,0);
	} );
}

//このコールバックに関連付けられているものをすべて消す
bool Recognition_Cloud::RemoveCallback(const CallbackDataStruct* callback , bool is_unrefCallback) 
{
	return true;
}
