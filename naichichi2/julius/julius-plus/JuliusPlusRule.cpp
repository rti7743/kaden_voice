/*
julius をMS-SAPIレベルぐらいに楽に使うために、我々はもっとハックをしなければならない。
ソースコードはご自由に。


julius「マナの本当の力を思い知れ!!」
*/
#include "JuliusPlusRule.h"
#include <assert.h>
#include <sstream>
#include <iostream>
#include <boost/algorithm/string.hpp>

//dictからマッチするルールを探す
const JuliusPlusRule* JuliusPlusRule::findDict(int dict)  const
{
	if ( this->dictNumber == dict )
	{
		return this;
	}
	for(auto it = this->nestrules.begin() ; it != this->nestrules.end() ; ++it)
	{
		auto p = (*it)->findDict(dict);
		if (p)
		{
			return p;
		}
	}
	if ( this->combineRule != NULL )
	{
		return this->combineRule->findDict(dict);
	}
	return NULL;
}

//読みからマッチする単語構造体を取得する
const JuliusPlusRule::wordnode* JuliusPlusRule::findYomi(const std::string& yomiPress) const
{
	for(auto wordIT = this->words.begin() ; wordIT != this->words.end() ; ++wordIT)
	{
		if ( (*wordIT)->yomiPress == yomiPress )
		{
			return *wordIT;
		}
	}
	for(auto it = this->nestrules.begin() ; it != this->nestrules.end() ; ++it)
	{
		auto p = (*it)->findYomi(yomiPress);
		if (p)
		{
			return p;
		}
	}
	if ( this->combineRule != NULL )
	{
		return this->combineRule->findYomi(yomiPress);
	}
	return NULL;
}

//このノードは自分の下位ノードですか？ (term除く)
bool JuliusPlusRule::IsMemberNode(const JuliusPlusRule* node,bool nest) const
{
	for(auto it = this->nestrules.begin() ; it != this->nestrules.end() ; ++it)
	{
		if ( *it == node )
		{
			return true;
		}
		if ( (*it)->IsMemberNode(node , true) )
		{
			return true;
		}
	}

	if (nest)
	{//子ノード探索時のみ termに触れる
		if ( this->combineRule != NULL )
		{
			return this->combineRule->IsMemberNode(node,true);
		}
	}
	return false;
}


//読みを作成する。
std::string JuliusPlusRule::convertYomi(const std::string& word) 
{
	std::string w = word;

	//この先地獄
	//ひらがな安宅な変換(愚直な方法で)
	boost::algorithm::replace_all(w , "ア","あ");
	boost::algorithm::replace_all(w , "イ","い");
	boost::algorithm::replace_all(w , "ウ","う");
	boost::algorithm::replace_all(w , "エ","え");
	boost::algorithm::replace_all(w , "オ","お");
	boost::algorithm::replace_all(w , "カ","か");
	boost::algorithm::replace_all(w , "キ","き");
	boost::algorithm::replace_all(w , "ク","く");
	boost::algorithm::replace_all(w , "ケ","け");
	boost::algorithm::replace_all(w , "コ","こ");
	boost::algorithm::replace_all(w , "サ","さ");
	boost::algorithm::replace_all(w , "シ","し");
	boost::algorithm::replace_all(w , "ス","す");
	boost::algorithm::replace_all(w , "セ","せ");
	boost::algorithm::replace_all(w , "ソ","そ");
	boost::algorithm::replace_all(w , "タ","た");
	boost::algorithm::replace_all(w , "チ","ち");
	boost::algorithm::replace_all(w , "ツ","つ");
	boost::algorithm::replace_all(w , "テ","て");
	boost::algorithm::replace_all(w , "ト","と");
	boost::algorithm::replace_all(w , "ナ","な");
	boost::algorithm::replace_all(w , "ニ","に");
	boost::algorithm::replace_all(w , "ヌ","ぬ");
	boost::algorithm::replace_all(w , "ネ","ね");
	boost::algorithm::replace_all(w , "ノ","の");
	boost::algorithm::replace_all(w , "ハ","は");
	boost::algorithm::replace_all(w , "ヒ","ひ");
	boost::algorithm::replace_all(w , "フ","ふ");
	boost::algorithm::replace_all(w , "ヘ","へ");
	boost::algorithm::replace_all(w , "ホ","ほ");
	boost::algorithm::replace_all(w , "マ","ま");
	boost::algorithm::replace_all(w , "ミ","み");
	boost::algorithm::replace_all(w , "ム","む");
	boost::algorithm::replace_all(w , "メ","め");
	boost::algorithm::replace_all(w , "モ","も");
	boost::algorithm::replace_all(w , "ヤ","や");
	boost::algorithm::replace_all(w , "ユ","ゆ");
	boost::algorithm::replace_all(w , "ヨ","よ");
	boost::algorithm::replace_all(w , "ヲ","を");
	boost::algorithm::replace_all(w , "ラ","ら");
	boost::algorithm::replace_all(w , "リ","り");
	boost::algorithm::replace_all(w , "ル","る");
	boost::algorithm::replace_all(w , "レ","れ");
	boost::algorithm::replace_all(w , "ロ","ろ");
	boost::algorithm::replace_all(w , "ン","ん");
	boost::algorithm::replace_all(w , "ヴ","う゛");
	boost::algorithm::replace_all(w , "ァ","ぁ");
	boost::algorithm::replace_all(w , "ィ","ぃ");
	boost::algorithm::replace_all(w , "ゥ","ぅ");
	boost::algorithm::replace_all(w , "ェ","ぇ");
	boost::algorithm::replace_all(w , "ォ","ぉ");
	boost::algorithm::replace_all(w , "ガ","が");
	boost::algorithm::replace_all(w , "ギ","ぎ");
	boost::algorithm::replace_all(w , "グ","ぐ");
	boost::algorithm::replace_all(w , "ゲ","げ");
	boost::algorithm::replace_all(w , "ゴ","ご");
	boost::algorithm::replace_all(w , "ザ","ざ");
	boost::algorithm::replace_all(w , "ジ","じ");
	boost::algorithm::replace_all(w , "ズ","ず");
	boost::algorithm::replace_all(w , "ゼ","ぜ");
	boost::algorithm::replace_all(w , "ゾ","ぞ");
	boost::algorithm::replace_all(w , "ダ","だ");
	boost::algorithm::replace_all(w , "ヂ","ぢ");
	boost::algorithm::replace_all(w , "ヅ","づ");
	boost::algorithm::replace_all(w , "デ","で");
	boost::algorithm::replace_all(w , "ド","ど");
	boost::algorithm::replace_all(w , "バ","ば");
	boost::algorithm::replace_all(w , "ビ","び");
	boost::algorithm::replace_all(w , "ブ","ぶ");
	boost::algorithm::replace_all(w , "ベ","べ");
	boost::algorithm::replace_all(w , "ボ","ぼ");
	boost::algorithm::replace_all(w , "パ","ぱ");
	boost::algorithm::replace_all(w , "ピ","ぴ");
	boost::algorithm::replace_all(w , "プ","ぷ");
	boost::algorithm::replace_all(w , "ペ","ぺ");
	boost::algorithm::replace_all(w , "ポ","ぽ");
	boost::algorithm::replace_all(w , "ャ","ゃ");
	boost::algorithm::replace_all(w , "ュ","ゅ");
	boost::algorithm::replace_all(w , "ョ","ょ");

	//特殊ルール
	boost::algorithm::replace_all(w , "え゛","えっ");
	boost::algorithm::replace_all(w , "こんにちは","こんにちわ");
	boost::algorithm::replace_all(w , "こんばんは","こんばんわ");
	boost::algorithm::replace_all(w , "わたしは","わたしわ");
	boost::algorithm::replace_all(w , "ぼくは","ぼくわ");
	boost::algorithm::replace_all(w , "かれは","かれわ");
	boost::algorithm::replace_all(w , "かれらは","かれらわ");
	boost::algorithm::replace_all(w , "かのじょは","かのじょわ");
	boost::algorithm::replace_all(w , "これらは","これらわ");
	boost::algorithm::replace_all(w , "あれは","あれわ");
	////boost::algorithm::replace_all(w , "なのは","なのわ"); //NLPの最大の課題、リリカルなのは問題によりダメです。

	//yomi2vocaより
	//# 3文字以上からなる変換規則（v a）
	boost::algorithm::replace_all(w , "う゛ぁ"," b a");
	boost::algorithm::replace_all(w , "う゛ぃ"," b i");
	boost::algorithm::replace_all(w , "う゛ぇ"," b e");
	boost::algorithm::replace_all(w , "う゛ぉ"," b o");
	boost::algorithm::replace_all(w , "う゛ゅ"," by u");

	//# 2文字からなる変換規則
    boost::algorithm::replace_all(w , "ぅ゛"," b u");
	boost::algorithm::replace_all(w , "あぁ"," a a");
	boost::algorithm::replace_all(w , "いぃ"," i i");
    boost::algorithm::replace_all(w , "いぇ"," i e");
	boost::algorithm::replace_all(w , "いゃ"," y a");
	boost::algorithm::replace_all(w , "うぅ"," u:");
	boost::algorithm::replace_all(w , "えぇ"," e e");
	boost::algorithm::replace_all(w , "おぉ"," o:");
	boost::algorithm::replace_all(w , "かぁ"," k a:");
	boost::algorithm::replace_all(w , "きぃ"," k i:");
	boost::algorithm::replace_all(w , "くぅ"," k u:");
	boost::algorithm::replace_all(w , "くゃ"," ky a");
	boost::algorithm::replace_all(w , "くゅ"," ky u");
	boost::algorithm::replace_all(w , "くょ"," ky o");
	boost::algorithm::replace_all(w , "けぇ"," k e:");
	boost::algorithm::replace_all(w , "こぉ"," k o:");
	boost::algorithm::replace_all(w , "がぁ"," g a:");
	boost::algorithm::replace_all(w , "ぎぃ"," g i:");
	boost::algorithm::replace_all(w , "ぐぅ"," g u:");
	boost::algorithm::replace_all(w , "ぐゃ"," gy a");
	boost::algorithm::replace_all(w , "ぐゅ"," gy u");
	boost::algorithm::replace_all(w , "ぐょ"," gy o");
	boost::algorithm::replace_all(w , "げぇ"," g e:");
	boost::algorithm::replace_all(w , "ごぉ"," g o:");
	boost::algorithm::replace_all(w , "さぁ"," s a:");
	boost::algorithm::replace_all(w , "しぃ"," sh i:");
	boost::algorithm::replace_all(w , "すぅ"," s u:");
	boost::algorithm::replace_all(w , "すゃ"," sh a");
	boost::algorithm::replace_all(w , "すゅ"," sh u");
	boost::algorithm::replace_all(w , "すょ"," sh o");
	boost::algorithm::replace_all(w , "せぇ"," s e:");
	boost::algorithm::replace_all(w , "そぉ"," s o:");
	boost::algorithm::replace_all(w , "ざぁ"," z a:");
	boost::algorithm::replace_all(w , "じぃ"," j i:");
	boost::algorithm::replace_all(w , "ずぅ"," z u:");
	boost::algorithm::replace_all(w , "ずゃ"," zy a");
	boost::algorithm::replace_all(w , "ずゅ"," zy u");
	boost::algorithm::replace_all(w , "ずょ"," zy o");
	boost::algorithm::replace_all(w , "ぜぇ"," z e:");
	boost::algorithm::replace_all(w , "ぞぉ"," z o:");
	boost::algorithm::replace_all(w , "たぁ"," t a:");
	boost::algorithm::replace_all(w , "ちぃ"," ch i:");
	boost::algorithm::replace_all(w , "つぁ"," ts a");
	boost::algorithm::replace_all(w , "つぃ"," ts i");
	boost::algorithm::replace_all(w , "つぅ"," ts u:");
	boost::algorithm::replace_all(w , "つゃ"," ch a");
	boost::algorithm::replace_all(w , "つゅ"," ch u");
	boost::algorithm::replace_all(w , "つょ"," ch o");
	boost::algorithm::replace_all(w , "つぇ"," ts e");
	boost::algorithm::replace_all(w , "つぉ"," ts o");
	boost::algorithm::replace_all(w , "てぇ"," t e:");
	boost::algorithm::replace_all(w , "とぉ"," t o:");
	boost::algorithm::replace_all(w , "だぁ"," d a:");
	boost::algorithm::replace_all(w , "ぢぃ"," j i:");
	boost::algorithm::replace_all(w , "づぅ"," d u:");
	boost::algorithm::replace_all(w , "づゃ"," zy a");
	boost::algorithm::replace_all(w , "づゅ"," zy u");
	boost::algorithm::replace_all(w , "づょ"," zy o");
	boost::algorithm::replace_all(w , "でぇ"," d e:");
	boost::algorithm::replace_all(w , "どぉ"," d o:");
	boost::algorithm::replace_all(w , "なぁ"," n a:");
	boost::algorithm::replace_all(w , "にぃ"," n i:");
	boost::algorithm::replace_all(w , "ぬぅ"," n u:");
	boost::algorithm::replace_all(w , "ぬゃ"," ny a");
	boost::algorithm::replace_all(w , "ぬゅ"," ny u");
	boost::algorithm::replace_all(w , "ぬょ"," ny o");
	boost::algorithm::replace_all(w , "ねぇ"," n e:");
	boost::algorithm::replace_all(w , "のぉ"," n o:");
	boost::algorithm::replace_all(w , "はぁ"," h a:");
	boost::algorithm::replace_all(w , "ひぃ"," h i:");
	boost::algorithm::replace_all(w , "ふぅ"," f u:");
	boost::algorithm::replace_all(w , "ふゃ"," hy a");
	boost::algorithm::replace_all(w , "ふゅ"," hy u");
	boost::algorithm::replace_all(w , "ふょ"," hy o");
	boost::algorithm::replace_all(w , "へぇ"," h e:");
	boost::algorithm::replace_all(w , "ほぉ"," h o:");
	boost::algorithm::replace_all(w , "ばぁ"," b a:");
	boost::algorithm::replace_all(w , "びぃ"," b i:");
	boost::algorithm::replace_all(w , "ぶぅ"," b u:");
	boost::algorithm::replace_all(w , "ふゃ"," hy a");
	boost::algorithm::replace_all(w , "ぶゅ"," by u");
	boost::algorithm::replace_all(w , "ふょ"," hy o");
	boost::algorithm::replace_all(w , "べぇ"," b e:");
	boost::algorithm::replace_all(w , "ぼぉ"," b o:");
	boost::algorithm::replace_all(w , "ぱぁ"," p a:");
	boost::algorithm::replace_all(w , "ぴぃ"," p i:");
	boost::algorithm::replace_all(w , "ぷぅ"," p u:");
	boost::algorithm::replace_all(w , "ぷゃ"," py a");
	boost::algorithm::replace_all(w , "ぷゅ"," py u");
	boost::algorithm::replace_all(w , "ぷょ"," py o");
	boost::algorithm::replace_all(w , "ぺぇ"," p e:");
	boost::algorithm::replace_all(w , "ぽぉ"," p o:");
	boost::algorithm::replace_all(w , "まぁ"," m a:");
	boost::algorithm::replace_all(w , "みぃ"," m i:");
	boost::algorithm::replace_all(w , "むぅ"," m u:");
	boost::algorithm::replace_all(w , "むゃ"," my a");
	boost::algorithm::replace_all(w , "むゅ"," my u");
	boost::algorithm::replace_all(w , "むょ"," my o");
	boost::algorithm::replace_all(w , "めぇ"," m e:");
	boost::algorithm::replace_all(w , "もぉ"," m o:");
	boost::algorithm::replace_all(w , "やぁ"," y a:");
	boost::algorithm::replace_all(w , "ゆぅ"," y u:");
	boost::algorithm::replace_all(w , "ゆゃ"," y a:");
	boost::algorithm::replace_all(w , "ゆゅ"," y u:");
	boost::algorithm::replace_all(w , "ゆょ"," y o:");
	boost::algorithm::replace_all(w , "よぉ"," y o:");
	boost::algorithm::replace_all(w , "らぁ"," r a:");
	boost::algorithm::replace_all(w , "りぃ"," r i:");
	boost::algorithm::replace_all(w , "るぅ"," r u:");
	boost::algorithm::replace_all(w , "るゃ"," ry a");
	boost::algorithm::replace_all(w , "るゅ"," ry u");
	boost::algorithm::replace_all(w , "るょ"," ry o");
	boost::algorithm::replace_all(w , "れぇ"," r e:");
	boost::algorithm::replace_all(w , "ろぉ"," r o:");
	boost::algorithm::replace_all(w , "わぁ"," w a:");
	boost::algorithm::replace_all(w , "をぉ"," o:");

	boost::algorithm::replace_all(w , "う゛"," b u");
	boost::algorithm::replace_all(w , "でぃ"," d i");
	boost::algorithm::replace_all(w , "でぇ"," d e:");
	boost::algorithm::replace_all(w , "でゃ"," dy a");
	boost::algorithm::replace_all(w , "でゅ"," dy u");
	boost::algorithm::replace_all(w , "でょ"," dy o");
	boost::algorithm::replace_all(w , "てぃ"," t i");
	boost::algorithm::replace_all(w , "てぇ"," t e:");
	boost::algorithm::replace_all(w , "てゃ"," ty a");
	boost::algorithm::replace_all(w , "てゅ"," ty u");
	boost::algorithm::replace_all(w , "てょ"," ty o");
	boost::algorithm::replace_all(w , "すぃ"," s i");
	boost::algorithm::replace_all(w , "ずぁ"," z u a");
	boost::algorithm::replace_all(w , "ずぃ"," z i");
	boost::algorithm::replace_all(w , "ずぅ"," z u");
	boost::algorithm::replace_all(w , "ずゃ"," zy a");
	boost::algorithm::replace_all(w , "ずゅ"," zy u");
	boost::algorithm::replace_all(w , "ずょ"," zy o");
	boost::algorithm::replace_all(w , "ずぇ"," z e");
	boost::algorithm::replace_all(w , "ずぉ"," z o");
	boost::algorithm::replace_all(w , "きゃ"," ky a");
	boost::algorithm::replace_all(w , "きゅ"," ky u");
	boost::algorithm::replace_all(w , "きょ"," ky o");
	boost::algorithm::replace_all(w , "しゃ"," sh a");
	boost::algorithm::replace_all(w , "しゅ"," sh u");
	boost::algorithm::replace_all(w , "しぇ"," sh e");
	boost::algorithm::replace_all(w , "しょ"," sh o");
	boost::algorithm::replace_all(w , "ちゃ"," ch a");
	boost::algorithm::replace_all(w , "ちゅ"," ch u");
	boost::algorithm::replace_all(w , "ちぇ"," ch e");
	boost::algorithm::replace_all(w , "ちょ"," ch o");
	boost::algorithm::replace_all(w , "とぅ"," t u");
	boost::algorithm::replace_all(w , "とゃ"," ty a");
	boost::algorithm::replace_all(w , "とゅ"," ty u");
	boost::algorithm::replace_all(w , "とょ"," ty o");
	boost::algorithm::replace_all(w , "どぁ"," d o a");
	boost::algorithm::replace_all(w , "どぅ"," d u");
	boost::algorithm::replace_all(w , "どゃ"," dy a");
	boost::algorithm::replace_all(w , "どゅ"," dy u");
	boost::algorithm::replace_all(w , "どょ"," dy o");
	boost::algorithm::replace_all(w , "どぉ"," d o:");
	boost::algorithm::replace_all(w , "にゃ"," ny a");
	boost::algorithm::replace_all(w , "にゅ"," ny u");
	boost::algorithm::replace_all(w , "にょ"," ny o");
	boost::algorithm::replace_all(w , "ひゃ"," hy a");
	boost::algorithm::replace_all(w , "ひゅ"," hy u");
	boost::algorithm::replace_all(w , "ひょ"," hy o");
	boost::algorithm::replace_all(w , "みゃ"," my a");
	boost::algorithm::replace_all(w , "みゅ"," my u");
	boost::algorithm::replace_all(w , "みょ"," my o");
	boost::algorithm::replace_all(w , "りゃ"," ry a");
	boost::algorithm::replace_all(w , "りゅ"," ry u");
	boost::algorithm::replace_all(w , "りょ"," ry o");
	boost::algorithm::replace_all(w , "ぎゃ"," gy a");
	boost::algorithm::replace_all(w , "ぎゅ"," gy u");
	boost::algorithm::replace_all(w , "ぎょ"," gy o");
	boost::algorithm::replace_all(w , "ぢぇ"," j e");
	boost::algorithm::replace_all(w , "ぢゃ"," j a");
	boost::algorithm::replace_all(w , "ぢゅ"," j u");
	boost::algorithm::replace_all(w , "ぢょ"," j o");
	boost::algorithm::replace_all(w , "じぇ"," j e");
	boost::algorithm::replace_all(w , "じゃ"," j a");
	boost::algorithm::replace_all(w , "じゅ"," j u");
	boost::algorithm::replace_all(w , "じょ"," j o");
	boost::algorithm::replace_all(w , "びゃ"," by a");
	boost::algorithm::replace_all(w , "びゅ"," by u");
	boost::algorithm::replace_all(w , "びょ"," by o");
	boost::algorithm::replace_all(w , "ぴゃ"," py a");
	boost::algorithm::replace_all(w , "ぴゅ"," py u");
	boost::algorithm::replace_all(w , "ぴょ"," py o");
	boost::algorithm::replace_all(w , "うぁ"," u a");
	boost::algorithm::replace_all(w , "うぃ"," w i");
	boost::algorithm::replace_all(w , "うぇ"," w e");
	boost::algorithm::replace_all(w , "うぉ"," w o");
	boost::algorithm::replace_all(w , "ふぁ"," f a");
	boost::algorithm::replace_all(w , "ふぃ"," f i");
	boost::algorithm::replace_all(w , "ふぅ"," f u");
	boost::algorithm::replace_all(w , "ふゃ"," hy a");
	boost::algorithm::replace_all(w , "ふゅ"," hy u");
	boost::algorithm::replace_all(w , "ふょ"," hy o");
	boost::algorithm::replace_all(w , "ふぇ"," f e");
	boost::algorithm::replace_all(w , "ふぉ"," f o");

	//# 1音からなる変換規則
	boost::algorithm::replace_all(w , "あ"," a");
	boost::algorithm::replace_all(w , "い"," i");
	boost::algorithm::replace_all(w , "う"," u");
	boost::algorithm::replace_all(w , "え"," e");
	boost::algorithm::replace_all(w , "お"," o");
	boost::algorithm::replace_all(w , "か"," k a");
	boost::algorithm::replace_all(w , "き"," k i");
	boost::algorithm::replace_all(w , "く"," k u");
	boost::algorithm::replace_all(w , "け"," k e");
	boost::algorithm::replace_all(w , "こ"," k o");
	boost::algorithm::replace_all(w , "さ"," s a");
	boost::algorithm::replace_all(w , "し"," sh i");
	boost::algorithm::replace_all(w , "す"," s u");
	boost::algorithm::replace_all(w , "せ"," s e");
	boost::algorithm::replace_all(w , "そ"," s o");
	boost::algorithm::replace_all(w , "た"," t a");
	boost::algorithm::replace_all(w , "ち"," ch i");
	boost::algorithm::replace_all(w , "つ"," ts u");
	boost::algorithm::replace_all(w , "て"," t e");
	boost::algorithm::replace_all(w , "と"," t o");
	boost::algorithm::replace_all(w , "な"," n a");
	boost::algorithm::replace_all(w , "に"," n i");
	boost::algorithm::replace_all(w , "ぬ"," n u");
	boost::algorithm::replace_all(w , "ね"," n e");
	boost::algorithm::replace_all(w , "の"," n o");
	boost::algorithm::replace_all(w , "は"," h a");
	boost::algorithm::replace_all(w , "ひ"," h i");
	boost::algorithm::replace_all(w , "ふ"," f u");
	boost::algorithm::replace_all(w , "へ"," h e");
	boost::algorithm::replace_all(w , "ほ"," h o");
	boost::algorithm::replace_all(w , "ま"," m a");
	boost::algorithm::replace_all(w , "み"," m i");
	boost::algorithm::replace_all(w , "む"," m u");
	boost::algorithm::replace_all(w , "め"," m e");
	boost::algorithm::replace_all(w , "も"," m o");
	boost::algorithm::replace_all(w , "ら"," r a");
	boost::algorithm::replace_all(w , "り"," r i");
	boost::algorithm::replace_all(w , "る"," r u");
	boost::algorithm::replace_all(w , "れ"," r e");
	boost::algorithm::replace_all(w , "ろ"," r o");
	boost::algorithm::replace_all(w , "が"," g a");
	boost::algorithm::replace_all(w , "ぎ"," g i");
	boost::algorithm::replace_all(w , "ぐ"," g u");
	boost::algorithm::replace_all(w , "げ"," g e");
	boost::algorithm::replace_all(w , "ご"," g o");
	boost::algorithm::replace_all(w , "ざ"," z a");
	boost::algorithm::replace_all(w , "じ"," j i");
	boost::algorithm::replace_all(w , "ず"," z u");
	boost::algorithm::replace_all(w , "ぜ"," z e");
	boost::algorithm::replace_all(w , "ぞ"," z o");
	boost::algorithm::replace_all(w , "だ"," d a");
	boost::algorithm::replace_all(w , "ぢ"," j i");
	boost::algorithm::replace_all(w , "づ"," z u");
	boost::algorithm::replace_all(w , "で"," d e");
	boost::algorithm::replace_all(w , "ど"," d o");
	boost::algorithm::replace_all(w , "ば"," b a");
	boost::algorithm::replace_all(w , "び"," b i");
	boost::algorithm::replace_all(w , "ぶ"," b u");
	boost::algorithm::replace_all(w , "べ"," b e");
	boost::algorithm::replace_all(w , "ぼ"," b o");
	boost::algorithm::replace_all(w , "ぱ"," p a");
	boost::algorithm::replace_all(w , "ぴ"," p i");
	boost::algorithm::replace_all(w , "ぷ"," p u");
	boost::algorithm::replace_all(w , "ぺ"," p e");
	boost::algorithm::replace_all(w , "ぽ"," p o");
	boost::algorithm::replace_all(w , "や"," y a");
	boost::algorithm::replace_all(w , "ゆ"," y u");
	boost::algorithm::replace_all(w , "よ"," y o");
	boost::algorithm::replace_all(w , "わ"," w a");
	boost::algorithm::replace_all(w , "ゐ"," i");
	boost::algorithm::replace_all(w , "ゑ"," e");
	boost::algorithm::replace_all(w , "ん"," N");
	boost::algorithm::replace_all(w , "っ"," q");
	boost::algorithm::replace_all(w , "ー",":");

	//# ここまでに処理されてない ぁぃぅぇぉ はそのまま大文字扱い
	boost::algorithm::replace_all(w , "ぁ"," a");
	boost::algorithm::replace_all(w , "ぃ"," i");
	boost::algorithm::replace_all(w , "ぅ"," u");
	boost::algorithm::replace_all(w , "ぇ"," e");
	boost::algorithm::replace_all(w , "ぉ"," o");
	boost::algorithm::replace_all(w , "ゎ"," w a");
	boost::algorithm::replace_all(w , "ぉ"," o");

	//# その他特別なルール
    boost::algorithm::replace_all(w , "を"," o");

	//# 変換の結果長音記号が続くことがまれにあるので一つにまとめる
	boost::algorithm::replace_all(w , ": : :",":");
	boost::algorithm::replace_all(w , ": :",":");

	//きゅう -> ky u u -> ky u: みたいに 連続する u u は u:にする
	//http://julius.sourceforge.jp/sapi/Docs/develop.html
//		boost::algorithm::replace_all(w ,"ああ" , "あー");
//		boost::algorithm::replace_all(w ,"いい" , "いー");
//		boost::algorithm::replace_all(w ,"うう" , "うー");
//		boost::algorithm::replace_all(w ,"ええ" , "えー");
//		boost::algorithm::replace_all(w ,"えい" , "えー");
//		boost::algorithm::replace_all(w ,"おお" , "おー");
//		boost::algorithm::replace_all(w ,"おう" , "おー");
	boost::algorithm::replace_all(w , " a a"," a:");
	boost::algorithm::replace_all(w , " i i"," i:");
	boost::algorithm::replace_all(w , " u u"," u:");
	boost::algorithm::replace_all(w , " e e"," e:");
	boost::algorithm::replace_all(w , " e i"," e:");
	boost::algorithm::replace_all(w , " o o"," o:");
	boost::algorithm::replace_all(w , " o u"," o:");

	//前後のスペースを削る
	boost::algorithm::trim(w);

	return w;
};

//空白なしにする
std::string JuliusPlusRule::converPress(const std::string& word) 
{
	return boost::algorithm::replace_all_copy( word , " ", "");
}

//読みを作成する(空白なし)
std::string JuliusPlusRule::convertYomiAndPress(const std::string& word) 
{
	return converPress( convertYomi(word));
}
