#pragma once
//liblinear を便利に使うためのライブラリ

//liblinear本体
#include "../liblinear/linear.h"

//もし、dllとしてロードするなら、これを立ててね
//#define _LIBLINEAR_WITH_DLL

class XLMachineLearningLibliear
{
public:
	XLMachineLearningLibliear()
	{
#ifdef _LIBLINEAR_WITH_DLL
		lib.Load("liblinear.dll");
		func_train = (def_train) lib.GetProcAddress("train");
		func_cross_validation = (def_cross_validation) lib.GetProcAddress("cross_validation");

		func_predict_values = (def_predict_values) lib.GetProcAddress("predict_values");
		func_predict = (def_predict) lib.GetProcAddress("predict");
		func_predict_probability = (def_predict_probability) lib.GetProcAddress("predict_probability");

		func_save_model = (def_save_model) lib.GetProcAddress("save_model");
		func_load_model = (def_load_model) lib.GetProcAddress("load_model");

		func_get_nr_feature = (def_get_nr_feature) lib.GetProcAddress("get_nr_feature");
		func_get_nr_class = (def_get_nr_class) lib.GetProcAddress("get_nr_class");
		func_get_labels = (def_get_labels) lib.GetProcAddress("get_labels");

		func_free_model_content = (def_free_model_content) lib.GetProcAddress("free_model_content");
		func_free_and_destroy_model = (def_free_and_destroy_model) lib.GetProcAddress("free_and_destroy_model");
		func_destroy_param = (def_destroy_param) lib.GetProcAddress("destroy_param");

		func_check_parameter = (def_check_parameter) lib.GetProcAddress("check_parameter");
		func_check_probability_model = (def_check_probability_model) lib.GetProcAddress("check_probability_model");
		func_set_print_string_function = (def_set_print_string_function) lib.GetProcAddress("set_print_string_function");
#else
		//仕方ないから関数ポインタでも渡しとくかw
		func_train = (def_train) train;
		func_cross_validation = (def_cross_validation) cross_validation;

		func_predict_values = (def_predict_values) predict_values;
		func_predict = (def_predict) predict;
		func_predict_probability = (def_predict_probability) predict_probability;

		func_save_model = (def_save_model) save_model;
		func_load_model = (def_load_model) load_model;

		func_get_nr_feature = (def_get_nr_feature) get_nr_feature;
		func_get_nr_class = (def_get_nr_class) get_nr_class;
		func_get_labels = (def_get_labels) get_labels;

		func_free_model_content = (def_free_model_content) free_model_content;
		func_free_and_destroy_model = (def_free_and_destroy_model) free_and_destroy_model;
		func_destroy_param = (def_destroy_param) destroy_param;

		func_check_parameter = (def_check_parameter) check_parameter;
		func_check_probability_model = (def_check_probability_model) check_probability_model;
		func_set_print_string_function = (def_set_print_string_function) set_print_string_function;
#endif
		this->Model = NULL;
	}
	virtual ~XLMachineLearningLibliear()
	{
		for(std::vector<feature_node*>::const_iterator it = ProblemX.begin() ; it != ProblemX.end() ; ++it )
		{
			delete [] *it;
		}
		FreeModel();
	}
	//ファイルから学習データを読み込む
	bool LoadTrain(const std::string& filename)
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
			feature_node* feature_nodes = new feature_node[featurecount + 1]; //終端もあるので +1する

			//クラス番号
			const int classid = atoi(startN);
			ProblemY.push_back(classid);
			ProblemX.push_back(feature_nodes);

			feature_node* featureP = feature_nodes;
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
					break;
				}
				else
				{
					*p = '\0';
					featureP->value = atof(startN);
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
	bool DebugPredict(const std::string& filename,const std::string& logFilename,int* outall,int* outmatch)
	{
		FILE * fp = fopen(filename.c_str() , "rb");
		if (!fp)
		{
			return false;
		}
		FILE * logfp = fopen(logFilename.c_str() , "wb");
		if (!logfp)
		{
			return false;
		}
		int all = 0;
		int match = 0;

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

			//どんな値だったかを保存しておきます。
			std::string feature_string = p + 1;

			//素性の数だけ確保(直列に確保しないとダメ)
			feature_node* feature_nodes = new feature_node[featurecount + 1]; //終端もあるので +1する

			//クラス番号
			const int classid = atoi(startN);

			feature_node* featureP = feature_nodes;
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
					break;
				}
				else
				{
					*p = '\0';
					featureP->value = atof(startN);
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
				fprintf(logfp,"OK %d %s" , predictClassID , feature_string.c_str() );
			}
			else
			{
				fprintf(logfp,"BAD %d %s" , predictClassID , feature_string.c_str() );
			}
			all ++;

			//メモリ解放を忘れずに。
			delete [] feature_nodes;
		}
		fclose(fp);

		fprintf(logfp,"\r\nAccuracy = %lf%% (%d/%d)" , ((double)match) * 100 / all  , match , all);
		*outall = all;
		*outmatch = match;
		return true;
	}
	//学習データの追加 (liblinear 辞退がオンラインできないらしいから仕方ないけども)
	void AppendTrain(int classID,const std::vector<feature_node>& newnodes)
	{
		feature_node* nodes = new feature_node[newnodes.size()+1]; //終端もあるので+1しときます。

		ProblemY.push_back(classID);
		ProblemX.push_back(nodes);

		for(std::vector<feature_node>::const_iterator it = newnodes.begin() ; it != newnodes.end() ; ++it )
		{
			*nodes++ = *it;
		}

		//終端マーク -1 を入れる。
		nodes->index = -1;
		nodes->index = 0;
	}
	//学習します。
	bool Train()
	{
		int maxindex = 0;
		for(std::vector<feature_node*>::const_iterator it = ProblemX.begin() ; it != ProblemX.end() ; ++it )
		{
			for(feature_node* nodes = *it; nodes->index != -1 ; ++nodes )
			{
				maxindex = max(nodes->index,maxindex);
			}
		}

		struct problem prob;
		prob.bias = -1;				//バイアス？初期化するときの引数で与えるらしいが・・・
		prob.l = ProblemY.size();	//行数
		prob.n = maxindex;	//一番でかい素性番号
		prob.y = &ProblemY[0];
		prob.x = &ProblemX[0];

		struct parameter param;
		param.solver_type = L2R_L2LOSS_SVC_DUAL;
		param.C = 1;
		param.eps = INFINITE; // あとで設定し直すよ
		param.nr_weight = 0;
		param.weight_label = NULL;
		param.weight = NULL;

		if(param.eps == INFINITE)
		{
			if(param.solver_type == L2R_LR || param.solver_type == L2R_L2LOSS_SVC)
				param.eps = 0.01;
			else if(param.solver_type == L2R_L2LOSS_SVC_DUAL || param.solver_type == L2R_L1LOSS_SVC_DUAL || param.solver_type == MCSVM_CS || param.solver_type == L2R_LR_DUAL)
				param.eps = 0.1;
			else if(param.solver_type == L1R_L2LOSS_SVC || param.solver_type == L1R_LR)
				param.eps = 0.01;
		}

		FreeModel();
		this->Model = func_train(&prob,&param);
		return this->Model != NULL;
	}
	//学習したモデルからクラス番号を取得します。
	int Predict(const std::vector<feature_node>& params)
	{
		if (params.size() <= 0)
		{
			return 0;
		}
		if ( (params.rbegin())->index == -1 )
		{
			return func_predict(this->Model , &params[0]);
		}
		else
		{
			std::vector<feature_node> params2 = params;
			feature_node fin;
			fin.index = -1;
			fin.value = 0;
			params2.push_back(fin);

			return func_predict(this->Model , &params2[0]);
		}
	}
	//学習したモデルからクラス番号を取得します。
	int Predict(const std::map<int,double>& params)
	{
		if (params.empty())
		{
			return 0;
		}

		std::vector<feature_node> nodes;
		nodes.resize(params.size() + 1 );

		feature_node * p = &nodes[0];
		for(std::map<int,double>::const_iterator it = params.begin() ; it != params.end() ; ++it , ++p)
		{
			p->index = it->first;
			p->value = it->second;
		}
		p->index = -1;
		p->value = 0;

		return func_predict(this->Model , &nodes[0]);
	}
	//学習したモデルからクラス番号を取得します。
	int Predict(const feature_node* params)
	{
		return func_predict(this->Model , params);
	}
	bool SaveModel(const std::string& filename)
	{
		func_save_model(filename.c_str(),this->Model);
//		save_model(filename.c_str(),this->Model);
		return true;
	}
	bool LoadModel(const std::string& filename)
	{
		FreeModel();
		this->Model = func_load_model(filename.c_str());
		return true;
	}
	void FreeModel()
	{
		if (this->Model)
		{
//			func_free_model_content(this->Model);
			func_free_and_destroy_model(&this->Model);
//			free_model_content(this->Model);
			this->Model = NULL;
		}
	}

private:
	std::vector<int> ProblemY;
	std::vector<feature_node*> ProblemX;
	std::vector<feature_node>  RealFeature;
	model* Model;

#ifdef _LIBLINEAR_WITH_DLL
	LoadLibraryHelper lib;
#endif
	typedef struct model* (*def_train)(const struct problem *prob, const struct parameter *param);
	typedef void (*def_cross_validation)(const struct problem *prob, const struct parameter *param, int nr_fold, int *target);

	typedef int (*def_predict_values)(const struct model *model_, const struct feature_node *x, double* dec_values);
	typedef int (*def_predict)(const struct model *model_, const struct feature_node *x);
	typedef int (*def_predict_probability)(const struct model *model_, const struct feature_node *x, double* prob_estimates);

	typedef int (*def_save_model)(const char *model_file_name, const struct model *model_);
	typedef struct model* (*def_load_model)(const char *model_file_name);

	typedef int (*def_get_nr_feature)(const struct model *model_);
	typedef int (*def_get_nr_class)(const struct model *model_);
	typedef void (*def_get_labels)(const struct model *model_, int* label);

	typedef void (*def_free_model_content)(struct model *model_ptr);
	typedef void (*def_free_and_destroy_model)(struct model **model_ptr_ptr);
	typedef void (*def_destroy_param)(struct parameter *param);

	typedef const char* (*def_check_parameter)(const struct problem *prob, const struct parameter *param);
	typedef int (*def_check_probability_model)(const struct model *model);
	typedef void (*def_set_print_string_function)(void (*print_func) (const char*));


	def_train func_train;
	def_cross_validation func_cross_validation;

	def_predict_values func_predict_values;
	def_predict func_predict;
	def_predict_probability func_predict_probability;

	def_save_model func_save_model;
	def_load_model func_load_model;

	def_get_nr_feature func_get_nr_feature;
	def_get_nr_class func_get_nr_class;
	def_get_labels func_get_labels;

	def_free_model_content func_free_model_content;
	def_free_and_destroy_model func_free_and_destroy_model;
	def_destroy_param func_destroy_param;

	def_check_parameter func_check_parameter;
	def_check_probability_model func_check_probability_model;
	def_set_print_string_function func_set_print_string_function;
};
