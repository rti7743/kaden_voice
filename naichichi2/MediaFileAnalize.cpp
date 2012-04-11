#include "common.h"
#include "MainWindow.h"
#include "ScriptRunner.h"
#include "MediaFileAnalize.h"
#include "XLStringUtil.h"
#include "windows_encoding.h"


MediaFileAnalize::MediaFileAnalize()
{
	this->Runner = NULL;
}

MediaFileAnalize::~MediaFileAnalize()
{
	this->PtrShell.Release();

	if (this->Runner)
	{
		delete this->Runner;
		this->Runner = NULL;
	}
}
xreturn::r<bool> MediaFileAnalize::Create(MainWindow* poolMainWindow,const std::string& luaCommand)
{
	this->PoolMainWindow = poolMainWindow;

	this->PtrShell.CreateInstance(__uuidof(Shell32::Shell));
	const std::string luafilename = XLStringUtil::pathcombine( poolMainWindow->Config.GetBaseDirectory() , luaCommand);

	//スクリプトのロード
	this->Runner = new ScriptRunner(poolMainWindow , false );
	this->Runner->LoadScript(luafilename);

	return true;

}

xreturn::r<bool> MediaFileAnalize::Analize(const std::string& dir,const std::string& filename , std::string* title,std::string* artist ,std::string* album , std::string* alias ,int * part,int * rank,  std::string* searchdata )
{
	*part = 0;
	*rank = 0;
	auto r1 = AnalizeLua(dir, filename , title, artist , album ,  alias , part ,rank);
	if ( ! r1  )
	{
		return xreturn::error(r1.getError());
	}
	auto r2 = AnalizeCOM(dir, filename , title, artist , album ,  alias , part ,rank);
	if ( ! r2  )
	{
		return xreturn::error(r2.getError());
	}

	//sqliteが日本語分かち書きに対応していないので、 バイグラムを作ってあげる
	*searchdata = MediaFileAnalize::makesearcableData(dir + " " + filename + " " + *title + " " + *artist + " " + *album + " " + *alias);

	//タイトルがないと表示した時にかっこわるいので、タイトルがないならファイル名を入れてあげる。
	if (makeUserSideSearchableData(*title) == "")
	{
		*title = XLStringUtil::basenameonly(filename);
	}

	return true;
}

xreturn::r<bool> MediaFileAnalize::AnalizeLua(const std::string& dir,const std::string& filename , std::string* title,std::string* artist ,std::string* album , std::string* alias ,int * part,int * rank)
{
	if ( this->Runner->IsMethodExist("title") )
	{
		*title += this->Runner->callFunction("title",dir,filename);
	}
	if ( this->Runner->IsMethodExist("artist") )
	{
		*artist += this->Runner->callFunction("artist",dir,filename);
	}
	if ( this->Runner->IsMethodExist("album") )
	{
		*album += this->Runner->callFunction("album",dir,filename);
	}
	if ( this->Runner->IsMethodExist("alias") )
	{
		*alias += this->Runner->callFunction("alias",dir,filename);
	}
	return true;
}


xreturn::r<bool> MediaFileAnalize::AnalizeCOM(const std::string& dir,const std::string& filename , std::string* title,std::string* artist ,std::string* album , std::string* alias ,int * part,int * rank)
{
	try
	{
		_variant_t var((short)Shell32::ssfRECENT);

		Shell32::FolderPtr ptrFolder = this->PtrShell->NameSpace( dir.c_str() );
		Shell32::FolderItemPtr ptrItem = ptrFolder->ParseName( filename.c_str() );

		std::string tmpartist;
		tmpartist = ptrFolder->GetDetailsOf( _variant_t((IDispatch *)ptrItem), 13); //アーティスト
		if (tmpartist == "") tmpartist = ptrFolder->GetDetailsOf( _variant_t((IDispatch *)ptrItem), 20);
		*artist		+= tmpartist;
		*title		+= ptrFolder->GetDetailsOf( _variant_t((IDispatch *)ptrItem), 21);	//タイトル
		*album		+= ptrFolder->GetDetailsOf( _variant_t((IDispatch *)ptrItem), 14);  //アルバムのタイトル
		*alias		+= ptrFolder->GetDetailsOf( _variant_t((IDispatch *)ptrItem), 16);	//ジャンル

		ptrItem.Release();
		ptrFolder.Release();
	}
	catch(_com_error& e)
	{
		return xreturn::error(std::string()+"com例外Shell32::Shell:" + e.ErrorMessage());
	}
	return true;
}


//検索可能データを作る
std::string MediaFileAnalize::makesearcableData(const std::string& str ) 
{
	std::string s = XLStringUtil::strtolower( XLStringUtil::chop(str) );
	{
		if ( this->Runner->IsMethodExist("makeplain") )
		{
			s = this->Runner->callFunction("makeplain",s);
		}
	}
	std::string cleaupData,yomi,yomiCleaupData,romajiKana,romaji,kana;

	//記号を消したデータを作ります。
	cleaupData = this->makeUserSideSearchableData(s);
	if (s == cleaupData) cleaupData = "";

	//クリーンアップした読みも取ります。
	yomiCleaupData =  this->PoolMainWindow->Mecab.KanjiAndKanakanaToHiragana(cleaupData);
	if (cleaupData == yomiCleaupData) yomiCleaupData = "";

	//漢字からよみがなを取ります。
	yomi = this->PoolMainWindow->Mecab.KanjiAndKanakanaToHiragana(s);
	if (yomi == yomiCleaupData) yomi = "";

	//ローマ字で記載された単語からひらがなよみを作り検索できるようにします。
	romajiKana = XLStringUtil::mb_convert_typo(yomiCleaupData,"R");
	if (yomiCleaupData == romajiKana) romajiKana = "";

	//ローマ字のままで入れた場合の補正
	romaji = XLStringUtil::mb_convert_typo(yomiCleaupData,"r");
	if (romaji == cleaupData) romaji = "";

	//かな入力で変換いれないで入力した場合の補正
	kana = XLStringUtil::mb_convert_typo(yomiCleaupData,"k");
	if (kana == cleaupData) kana = "";

	//元のデータ + 記号消したデータ + 記号消した読み + 読み + ローマ字 + カナ
	//バイグラムの形成
	return XLStringUtil::join(" ", XLStringUtil::unique(	XLStringUtil::makebigram(   
		str + " " + cleaupData + " " + yomiCleaupData + " " + yomi + " " + romajiKana + " " + romaji + " " + kana
		) ) );
}

std::string MediaFileAnalize::makeListToBigram(const std::list<std::string>& list ) const
{
	return XLStringUtil::join(" ", 
			XLStringUtil::unique( 
				XLStringUtil::array_map(list , [](const std::string& _) -> std::string 
					{
						return XLStringUtil::join(" ", XLStringUtil::unique(XLStringUtil::makebigram(_)));
					} 
				) 
		)
	);
}

//リストの中身を正規化します。
std::list<std::string> MediaFileAnalize::makeListToCleanup(const std::list<std::string>& list ) const
{
	return 
		XLStringUtil::array_map(list , [this](const std::string& _) -> std::string 
			{
				return this->makeUserSideSearchableData(_);
			} 
		) ;
		
}
//ユーザサイドが検索に使うデータを作る
std::string MediaFileAnalize::makeUserSideSearchableData(const std::string& str ) const
{
	std::string s = XLStringUtil::chop(str);

	//いらないキャラクター
	const char* removeCharTable[] = {
		"!","\"","#","$","%","&","'","(",")","=","-","~","^","|","\\","[","]","{","}","+",";","*",":","?",",",".","<",">","/"
		,"。","、","､","｡","★","☆","※","・"
		,NULL //終端
	};
	//読み方の正規化
	s = XLStringUtil::mb_convert_kana(s,"cHsa");
	//不要な記号の除去
	s = XLStringUtil::remove( s , removeCharTable );
	return s;
}
