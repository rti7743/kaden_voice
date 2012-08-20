#pragma once
#include <string>
#include <vector>
#include <list>
#include <map>

struct model;

//liblinear を便利に使うためのライブラリ
class XLMachineLearningLibliear
{
public:
	struct feature {
		int index;
		double value;
	};

	XLMachineLearningLibliear();
	
	virtual ~XLMachineLearningLibliear();

	//ファイルから学習データを読み込む
	bool LoadTrain(const std::string& filename);

	//教え込んだときと同じファイルフォーマットを使って、認識率を測定します。 デバッグ用
	bool DebugPredict(const std::string& filename,int* outall,int* outmatch,int* outDcount,std::list<std::string> *outbadfilelist);

	//学習データの追加 (liblinear 辞退がオンラインできないらしいから仕方ないけども)
	void AppendTrain(int classID,const std::vector<XLMachineLearningLibliear::feature>& newnodes);

	//学習します。
	bool Train();

	//学習したモデルからクラス番号を取得します。
	int Predict(const std::vector<feature>& params);

	//学習したモデルからクラス番号を取得します。
	int Predict(const std::map<int,double>& params);

	//学習したモデルからクラス番号を取得します。
	int Predict(const XLMachineLearningLibliear::feature* params);

	bool SaveModel(const std::string& filename);

	bool LoadModel(const std::string& filename);

	void FreeModel();
private:
	std::vector<int> ProblemY;
	std::vector<XLMachineLearningLibliear::feature*> ProblemX;
	std::vector<XLMachineLearningLibliear::feature>  RealFeature;
	model* Model;
};
