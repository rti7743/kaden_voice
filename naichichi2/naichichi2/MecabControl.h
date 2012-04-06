#pragma once
#include "../mecab/src/mecab.h"

class MecabControl
{
public:
	MecabControl();
	virtual ~MecabControl();
	xreturn::r<bool> MecabControl::Create();
	xreturn::r<bool> Create(const std::string& dicpath);
	void Parse(const std::string& str,const std::function< void(const MeCab::Node* node) >& callbackNode);

	//Š¿Žš‚»‚Ì‘¼‚ð‚·‚×‚Ä‚Ð‚ç‚ª‚È‚É’¼‚µ‚Ü‚·
	std::string MecabControl::KanjiAndKanakanaToHiragana(const std::string& str);

private:
	MeCab::Tagger *Tagger;
	mutable boost::mutex lock;
};
