#include "common.h"
#include "MecabControl.h"
#include "XLStringUtil.h"

MecabControl::MecabControl(void)
{
	this->Tagger = NULL;
}


MecabControl::~MecabControl(void)
{
	if (this->Tagger)
	{
		delete this->Tagger;
		this->Tagger = NULL;
	}
}

xreturn::r<bool> MecabControl::Create(const std::string& dicpath)
{
	assert(this->Tagger == NULL);

	std::string option = std::string("-d ") + dicpath;
	this->Tagger = MeCab::createTagger(option.c_str());
	if (this->Tagger == NULL)
	{
		return xreturn::error("mecab のインスタンスを作れませんでした");
	}
	return true;
}

void MecabControl::Parse(const std::string& str,const std::function< void(const MeCab::Node* node) >& callbackNode)
{
	boost::unique_lock<boost::mutex> al(this->lock);

	const MeCab::Node* node = this->Tagger->parseToNode( str.c_str() );
	if (node == NULL)
	{
		return ;
	}
	for (; node; node = node->next) 
	{
		callbackNode(node);
	}

}

//漢字その他をすべてひらがなに直します
std::string MecabControl::KanjiAndKanakanaToHiragana(const std::string& str)
{
	std::string yomi;
	this->Parse(str , [&](const MeCab::Node* node){
		std::vector<std::string> kammalist = XLStringUtil::split_vector(",",node->feature);
		if (kammalist.size() > 7 && kammalist[7] != "*")
		{
			yomi += kammalist[7];
		}
		else
		{
			yomi += std::string(node->surface, 0,node->length);
		}
	});
	//mecabだとカタカナ読みしか取れないので、強制的にひらがなに直します。
	return XLStringUtil::mb_convert_kana(yomi,"cHsa");
}