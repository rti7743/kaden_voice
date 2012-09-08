#include "common.h"
#include "MecabControl.h"
#include "XLStringUtil.h"

MecabControl::MecabControl(void)
{
	this->mecab = (Mecab*)malloc(sizeof(Mecab));
	Mecab_initialize(this->mecab);
}


MecabControl::~MecabControl(void)
{
	Mecab_clear(this->mecab);
}

bool MecabControl::Create(const std::string& dicpath)
{
	std::string option = std::string("-d ") + dicpath + "/";
	Mecab_load(this->mecab,(char*) option.c_str());
	return true;
}

/*
void MecabControl::Parse(const std::string& str,const std::function< void(const MeCab::Node* node) >& callbackNode)
{
	boost::unique_lock<boost::mutex> al(this->lock);
   Mecab_analysis(&this->mecab, str.c_str() );
   mecab2njd(&open_jtalk->njd, Mecab_get_feature(&open_jtalk->mecab),
             Mecab_get_size(&open_jtalk->mecab));

	const MeCab::Node* node = this->Tagger->parseToNode( str.c_str() );
	if (node == NULL)
	{
		return ;
	}
	for (; node; node = node->next) 
	{
		callbackNode(node);
	}

	Mecab_refresh(&this->mecab);
}
*/

//漢字その他をすべてひらがなに直します
std::string MecabControl::KanjiAndKanakanaToHiragana(const std::string& str)
{
	std::string yomi;
/*
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
*/
	//mecabだとカタカナ読みしか取れないので、強制的にひらがなに直します。
	return XLStringUtil::mb_convert_kana(yomi,"cHsa");
}
