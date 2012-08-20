#include "XLMachineLearningLibSVM.h"
//liblinear本体
#include "../libsvm/svm.h"
#include <algorithm>


#if _MSC_VER
	#pragma comment(lib, "libsvm.lib")
#endif

#if !defined(max)
#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif


//デバッグ用
static double getFeatureValue(const XLMachineLearningLibSVM::feature* feature_nodes , int index) 
{
	const XLMachineLearningLibSVM::feature* featureP = feature_nodes;
	for( ; 1 ; ++featureP )
	{
		if ( featureP == NULL || featureP->index == -1 ) break;
		if (featureP->index == index)
		{
			return featureP->value;
		}
	}
	return 0;
}

static bool IgnoreFeature(int index) 
{
	return false;

	//特定の素性を無効に
	if (index >= 10 
		&& 
		!(
//		(((index - 10) % 25) >= 0 &&  ((index - 10) % 25) <= 11 )
//		(((index - 10) % 25) >= 0 &&  ((index - 10) % 25) <= 12 )
		(((index - 10) % 25) == 12 )
//		(((index - 10) % 25) >= 13 &&  ((index - 10) % 25) <= 24 )

//		(((index - 10) % 25) >= 0 &&  ((index - 10) % 25) <= 11 )
//||		(((index - 10) % 25) >= 12 &&  ((index - 10) % 25) <= 23 )
//		(((index - 10) % 25) >= 0 &&  ((index - 10) % 25) <= 11 )
//||		(((index - 10) % 25) >= 13 &&  ((index - 10) % 25) <= 24 )
//		(((index - 10) % 25) >= 0 &&  ((index - 10) % 25) <= 11 )
//||		(((index - 10) % 25) == 12 )
//		(((index - 10) % 25) >= 0 &&  ((index - 10) % 25) <= 11 )
//||		(((index - 10) % 25) == 24 )
//		(((index - 10) % 25) >= 13 &&  ((index - 10) % 25) <= 24 )
//||		(((index - 10) % 25) == 12 )
//		0
		) 
	) 
			return true;

	return false;
}




XLMachineLearningLibSVM::XLMachineLearningLibSVM()
{
	this->Model = NULL;
}

XLMachineLearningLibSVM::~XLMachineLearningLibSVM()
{
	for(std::vector<XLMachineLearningLibSVM::feature*>::const_iterator it = ProblemX.begin() ; it != ProblemX.end() ; ++it )
	{
		delete [] *it;
	}
	FreeModel();
}

//ファイルから学習データを読み込む
bool XLMachineLearningLibSVM::LoadTrain(const std::string& filename)
{
	FILE * fp = fopen(filename.c_str() , "rb");
	if (!fp)
	{
		return false;
	}

	std::vector<char> buffer;
	buffer.resize(65535);
	while (! feof(fp) )
	{
		char * p = &buffer[0];
		fgets(p,buffer.size(), fp);
		if (*p == '#' || *p == '\n' || *p == '\0') continue; //コメントとか

		char * startN = p;

		//何個 : があるかを数えます。
		int featurecount = 0;
		for( ; *startN  ; ++startN ) 
		{
			if (*startN == ':') featurecount ++;
		}

		//クラス番号との区切り線
		startN = p;
		for( ; *p != ' ' && *p != '\t' && *p != '\n' && *p != '\0'; ++p );
		if (*p == '\0' || *p == '\n' ) continue;
		*p = '\0';

		//素性の数だけ確保(直列に確保しないとダメ)
		feature* feature_nodes = new feature[featurecount + 1]; //終端もあるので +1する

		//クラス番号
		const int classid = atoi(startN);
		ProblemY.push_back((double)classid);
		ProblemX.push_back(feature_nodes);

		feature* featureP = feature_nodes;
		for( ; 1 ; ++featureP )
		{
			//素性番号
			p ++;
			startN = p;
			for( ; *p != ':' && *p != '\n'  && *p != '\0'; ++p );
			if (*p == '\0' || *p == '\n' ) break;
			*p = '\0';
			featureP->index = atoi(startN);

			//素性の値
			p ++;
			startN = p;
			for( ; *p != ' ' && *p != '\t' && *p != '\n'  && *p != '\0'; ++p );
			if (*p == '\0' || *p == '\n' )
			{
				featureP->value = atof(startN);
if ( IgnoreFeature(featureP->index) ) { featureP--; }
				break;
			}
			else
			{
				*p = '\0';
				featureP->value = atof(startN);
if ( IgnoreFeature(featureP->index) ) { featureP--; }
			}
		}
		//終端マーク
		featureP->index = -1;
		featureP->value = 0;
	}
	fclose(fp);
	return true;
}
//教え込んだときと同じファイルフォーマットを使って、認識率を測定します。 デバッグ用
bool XLMachineLearningLibSVM::DebugPredict(const std::string& filename,int* outall,int* outmatch,int* outDcount,std::list<std::string> *outbadfilelist)
{
	FILE * fp = fopen(filename.c_str() , "rb");
	if (!fp)
	{
		return false;
	}

	int all = 0;
	int match = 0;
	int Dcount = 0;

	std::string procfilename;
	std::vector<char> buffer;
	buffer.resize(262144);
	while (! feof(fp) )
	{
		char * p = &buffer[0];
		fgets(p,buffer.size(), fp);
		if (*p == '#' || *p == '\n' || *p == '\0')
		{//コメントはそのまま出力する.
			if (*p == '#')
			{
				procfilename = p + 2;
			}
			continue;
		}

		char * startN = p;

		//何個 : があるかを数えます。
		int featurecount = 0;
		for( ; *startN  ; ++startN ) 
		{
			if (*startN == ':') featurecount ++;
		}

		//クラス番号との区切り線
		startN = p;
		for( ; *p != ' ' && *p != '\t' && *p != '\n' && *p != '\0'; ++p );
		if (*p == '\0' || *p == '\n' ) continue;
		*p = '\0';

		//どんな値だったかを保存しておきます。
		std::string feature_string = p + 1;

		//素性の数だけ確保(直列に確保しないとダメ)
		feature* feature_nodes = new feature[featurecount + 1]; //終端もあるので +1する

		//クラス番号
		const int classid = atoi(startN);

		feature* featureP = feature_nodes;
		for( ; 1 ; ++featureP )
		{
			//素性番号
			p ++;
			startN = p;
			for( ; *p != ':' && *p != '\n'  && *p != '\0'; ++p );
			if (*p == '\0' || *p == '\n' ) break;
			*p = '\0';
			featureP->index = atoi(startN);

			//素性の値
			p ++;
			startN = p;
			for( ; *p != ' ' && *p != '\t' && *p != '\n'  && *p != '\0'; ++p );
			if (*p == '\0' || *p == '\n' )
			{
				featureP->value = atof(startN);
if ( IgnoreFeature(featureP->index) ) { featureP--; }
				break;
			}
			else
			{
				*p = '\0';
				featureP->value = atof(startN);
if ( IgnoreFeature(featureP->index) ) { featureP--; }
			}
		}
		//終端マーク
		featureP->index = -1;
		featureP->value = 0;

		//識別されてみます。
		int predictClassID = this->Predict(feature_nodes);

		//結果を測定します。
		if (predictClassID == classid)
		{
			match ++;
		}
		else
		{
			outbadfilelist->push_back( procfilename );

			if ( getFeatureValue(feature_nodes,8) <= 201 )
			{
				Dcount ++;
			}
		}
		all ++;

		//メモリ解放を忘れずに。
		delete [] feature_nodes;
		//現在処理しているファイル名を開放
		procfilename.clear();
	}
	fclose(fp);

	*outall = all;
	*outmatch = match;
	*outDcount = Dcount;
	return true;
}



//学習データの追加 (liblinear 辞退がオンラインできないらしいから仕方ないけども)
void XLMachineLearningLibSVM::AppendTrain(int classID,const std::vector<XLMachineLearningLibSVM::feature>& newnodes)
{
	feature* nodes = new feature[newnodes.size()+1]; //終端もあるので+1しときます。

	ProblemY.push_back((double)classID);
	ProblemX.push_back(nodes);

	for(std::vector<feature>::const_iterator it = newnodes.begin() ; it != newnodes.end() ; ++it )
	{
		*nodes++ = *it;
	}

	//終端マーク -1 を入れる。
	nodes->index = -1;
	nodes->index = 0;
}

//学習します。
bool XLMachineLearningLibSVM::Train()
{
	struct svm_problem prob;
	prob.l = ProblemY.size();	//行数
	prob.y = &ProblemY[0];
	prob.x = (::svm_node**) ((feature*) &ProblemX[0]);

	struct svm_parameter param;
	// default values
	param.svm_type = C_SVC;
	param.kernel_type = RBF;
	param.degree = 3;
	param.gamma = 0;	// 1/num_features
	param.coef0 = 0;
	param.nu = 0.5;
	param.cache_size = 100;
	param.C = 1;
	param.eps = 1e-3;
	param.p = 0.1;
	param.shrinking = 1;
	param.probability = 0;
	param.nr_weight = 0;
	param.weight_label = NULL;
	param.weight = NULL;

	if (param.gamma == 0)
	{
		double maxindex = 0;
		for(std::vector<feature*>::const_iterator it = ProblemX.begin() ; it != ProblemX.end() ; ++it )
		{
			for(feature* nodes = *it; nodes->index != -1 ; ++nodes )
			{
				maxindex = max(nodes->index,maxindex);
			}
		}
		param.gamma = 1.0/maxindex;
	}

	//途中経過を表示しない
	svm_set_print_string_function(NULL);

	//パラメタのチェック
	const char *error_msg = svm_check_parameter(&prob,&param);
	if (error_msg)
	{
		return false;
	}

	FreeModel();
	this->Model = svm_train(&prob,&param);
	return this->Model != NULL;
}

//学習したモデルからクラス番号を取得します。
int XLMachineLearningLibSVM::Predict(const std::vector<feature>& params)
{
	if (params.size() <= 0)
	{
		return 0;
	}
	if ( (params.rbegin())->index == -1 )
	{
		return (int) svm_predict(this->Model , (::svm_node*) ((feature*) &params[0] ) );
	}
	else
	{
		std::vector<feature> params2 = params;
		feature fin;
		fin.index = -1;
		fin.value = 0;
		params2.push_back(fin);

		return (int) svm_predict(this->Model ,(::svm_node*) ((feature*) &params2[0] ) );
	}
}
//学習したモデルからクラス番号を取得します。
int XLMachineLearningLibSVM::Predict(const std::map<int,double>& params)
{
	if (params.empty())
	{
		return 0;
	}

	std::vector<feature> nodes;
	nodes.resize(params.size() + 1 );

	feature * p = &nodes[0];
	for(std::map<int,double>::const_iterator it = params.begin() ; it != params.end() ; ++it , ++p)
	{
		p->index = it->first;
		p->value = it->second;
	}
	p->index = -1;
	p->value = 0;

	return (int) svm_predict(this->Model , (::svm_node*) ((feature*) &nodes[0] ) );
}

//学習したモデルからクラス番号を取得します。
int XLMachineLearningLibSVM::Predict(const XLMachineLearningLibSVM::feature* params)
{
	return (int) svm_predict(this->Model , (::svm_node*) params  );
}

bool XLMachineLearningLibSVM::SaveModel(const std::string& filename)
{
	svm_save_model(filename.c_str(),this->Model);
//		svm_save_model(filename.c_str(),this->Model);
	return true;
}

bool XLMachineLearningLibSVM::LoadModel(const std::string& filename)
{
	FreeModel();
	this->Model = svm_load_model(filename.c_str());
	return true;
}
void XLMachineLearningLibSVM::FreeModel()
{
	if (this->Model)
	{
		svm_free_model_content(this->Model);
//			svm_free_model_content(this->Model);
		this->Model = NULL;
	}
}


