#pragma once
#include "../mecab/src/mecab.h"

class MecabControl
{
public:
	MecabControl();
	virtual ~MecabControl();
	xreturn::r<bool> Create(const std::string& dicpath);
	void Parse(const std::string& str,const std::function< void(const MeCab::Node* node) >& callbackNode);
private:
	MeCab::Tagger *Tagger;
};
