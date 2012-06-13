#pragma once
#include "../mecab/src/mecab.h"

class MecabControl
{
public:
	MecabControl();
	virtual ~MecabControl();
	xreturn::r<bool> Create();
	xreturn::r<bool> Create(const std::string& dicpath);
	void Parse(const std::string& str,const std::function< void(const MeCab::Node* node) >& callbackNode);

	//漢字その他をすべてひらがなに直します
	std::string KanjiAndKanakanaToHiragana(const std::string& str);

private:
	MeCab::Tagger *Tagger;
	mutable boost::mutex lock;
};
