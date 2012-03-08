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
	const std::string luafilename = poolMainWindow->Config.GetBaseDirectory() + "\\" + luaCommand;

	//スクリプトのロード
	this->Runner = new ScriptRunner(poolMainWindow , false );
	this->Runner->LoadScript(luafilename);
	return true;

}

bool MediaFileAnalize::Analize(const std::string& dir,const std::string& filename , std::string* title,std::string* artist ,std::string* album , std::string* alias ,int * part,int * rank,  std::string* searchdata )
{
	*part = 0;
	*rank = 0;
	if ( ! AnalizeLua(dir, filename , title, artist , album ,  alias , part ,rank) )
	{
		return false;
	}
	if ( ! AnalizeCOM(dir, filename , title, artist , album ,  alias , part ,rank) )
	{
		return false;
	}

	//sqliteが日本語分かち書きに対応していないので、 バイグラムを作ってあげる
	*searchdata = MediaFileAnalize::makesearcableData(filename + " " + *title + " " + *artist + " " + *album + " " + *alias);

	//タイトルがないと表示した時にかっこわるいので、タイトルがないならファイル名を入れてあげる。
	if (title->empty())
	{
		*title = XLStringUtil::basenameonly(filename);
	}

	return true;
}

bool MediaFileAnalize::AnalizeLua(const std::string& dir,const std::string& filename , std::string* title,std::string* artist ,std::string* album , std::string* alias ,int * part,int * rank)
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


bool MediaFileAnalize::AnalizeCOM(const std::string& dir,const std::string& filename , std::string* title,std::string* artist ,std::string* album , std::string* alias ,int * part,int * rank)
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

	return true;
}

//検索可能データを作る
std::string MediaFileAnalize::makesearcableData(const std::string& str ) 
{
	std::string s = XLStringUtil::chop(str);
	{
		if ( this->Runner->IsMethodExist("makeplain") )
		{
			s = this->Runner->callFunction("makeplain",s);
		}
	}
	//漢字からよみがなを取ります。
	std::string yomi =  XLStringUtil::KanjiAndKanakanaToHiragana(str);
	//よみがなも含めたサーチ可能なデータを作ります。
	//バイグラムの形成
	return XLStringUtil::join(" ", XLStringUtil::unique(	XLStringUtil::makebigram(   makeUserSideSearchableData(yomi + " " + str) ) ) );
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
