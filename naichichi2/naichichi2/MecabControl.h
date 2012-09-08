#pragma once

//extern "C" {
	#include "../openjtalk/open_jtalk/mecab/src/mecab.h"
//};

class MecabControl
{
public:
	MecabControl();
	virtual ~MecabControl();
	bool Create();
	bool Create(const std::string& dicpath);
//	void Parse(const std::string& str,const std::function< void(const MeCab::Node* node) >& callbackNode);

	//漢字その他をすべてひらがなに直します
	std::string KanjiAndKanakanaToHiragana(const std::string& str);

	Mecab* getMecabHandle()
	{
		return this->mecab;
	}
	
private:
	Mecab* mecab;
	mutable boost::mutex lock;
};
