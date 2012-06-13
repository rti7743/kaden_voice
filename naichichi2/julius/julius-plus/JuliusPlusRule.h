#pragma once
#include <string>
#include <list>
#include <assert.h>
#include <algorithm>
#include <functional>

//コールバック
struct CallbackDataStruct
{
	CallbackDataStruct( )
	{
		Enable = false;
//		callbackFunctionalObject = [](){};
	}
	CallbackDataStruct( std::function<void (void)> f)  
	{
		Enable = true;
		callbackFunctionalObject = f;
	}
	CallbackDataStruct( const CallbackDataStruct& copy)
	{
		this->Enable = copy.Enable;
		this->callbackFunctionalObject = copy.callbackFunctionalObject;
	}

	bool Enable;
	std::function<void (void)> callbackFunctionalObject;
};

//JuliusPlusのルール
class JuliusPlusRule
{
public:
	struct wordnode
	{
		wordnode(const std::string& word,const std::string& yomi) : word(word) , yomi(yomi)
		{
			this->yomiPress = JuliusPlusRule::converPress(yomi);
		}

		//単語
		std::string word;
		//単語の読み
		std::string yomi;
		//単語の読みから空白をとったもの(結構参照するので作っておく)
		std::string yomiPress;
	};
private:
	//このルールの単語 いづれかにマッチする必要がある。
	std::list<wordnode*> words;
	//このルールの単語のいづれかにマッチした時に呼び出す階層のルール
	std::list<JuliusPlusRule*> nestrules;
	//このルールが終了したときに呼び出すデストラクタ的なルール
	JuliusPlusRule* combineRule;

	//julius の dict に割り振った値 無効値:0
	int dictNumber;	
	//正規表現 capture で何番目かの目印。 無効値:0
	int captureNumber;
	//ユーザルーチンへコーバックするものを保持する。
	CallbackDataStruct callback;

public:

	JuliusPlusRule(const CallbackDataStruct& c) : combineRule(NULL),callback(c) , dictNumber(0) , captureNumber(0)
	{
	}
	JuliusPlusRule(int capture,const CallbackDataStruct& c) : combineRule(NULL),callback(c) , dictNumber(0) , captureNumber(capture)
	{
	}
	virtual ~JuliusPlusRule()
	{
		this->Clear();
	}
	bool AddWord(const std::string& w )
	{
		this->words.push_back(new wordnode(w,convertYomi(w) ) );
		return true;
	}
	bool AddWord(const std::string& w , const std::string& yomi )
	{
		this->words.push_back(new wordnode(w,yomi) );
		return true;
	}
	bool AddNestRule(JuliusPlusRule* haveRule)
	{
		this->nestrules.push_back(haveRule);
		return true;
	}
	bool SetCombineRule(JuliusPlusRule* combineRule)
	{
		assert(this->combineRule == NULL);
		this->combineRule = combineRule;
		return true;
	}
	void RemoveNestRule(JuliusPlusRule* haveRule)
	{
		std::remove_if(this->nestrules.begin(),this->nestrules.end() , [&](JuliusPlusRule* r) -> bool { if (r == haveRule){ delete r; return true; }else{ return false; } } );
	}
	void RemoveWord(std::string word)
	{
		std::remove_if(this->words.begin(),this->words.end() , [&](wordnode* w) -> bool { if (w->word == word){ delete w; return true; }else{ return false; } } );
	}
	//dict番号は建前上は連番で振りたいので、全ノードをまとめてcommitしたときに連番に振り直すので、振ってみなければわからない
	//そのため、コンストラクタではなく、commitするときにまとめてアップデートする必要がある。
	void UpdateDictNumber(int dictNumber)
	{
		this->dictNumber = dictNumber;
	}
	void Clear()
	{
		{
			for(auto it = this->words.begin() ; it != this->words.end() ; ++it)
			{
				delete *it;
			}
			this->words.clear();
		}
		{
			for(auto it = this->nestrules.begin() ; it != this->nestrules.end() ; ++it)
			{
				delete *it;
			}
			this->nestrules.clear();
		}
		delete this->combineRule;
		this->combineRule = NULL;
	}
	const std::list<wordnode*>* getWords() const
	{
		return &this->words;
	}
	const std::list<JuliusPlusRule*>* getNestRules() const
	{
		return &this->nestrules;
	}
	JuliusPlusRule* getTermRule() const
	{
		return this->combineRule;
	}
	const CallbackDataStruct* getCallback() const
	{
		return &this->callback;
	}
	int getDictNumber() const
	{
		return this->dictNumber;
	}
	int getCaptureNumber() const
	{
		return this->captureNumber;
	}
	//dictからマッチするルールを探す
	const JuliusPlusRule* findDict(int dict) const;
	//読みからマッチする単語構造体を取得する
	const wordnode* findYomi(const std::string& yomiPress) const;
	//このノードは自分の下位ノードですか？ (term除く)
	bool IsMemberNode(const JuliusPlusRule* node,bool nest = false) const;
	//読みを作成する。
	static std::string convertYomi(const std::string& word) ;
	//読みを作成する(空白なし)
	static std::string convertYomiAndPress(const std::string& word) ;
	//空白なしにする
	static std::string converPress(const std::string& word) ;
};

