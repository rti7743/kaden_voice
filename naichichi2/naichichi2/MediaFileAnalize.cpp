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
	this->PtrShell.CreateInstance(__uuidof(Shell32::Shell));

	const std::string luafilename = poolMainWindow->Config.GetBaseDirectory() + "\\" + luaCommand;

	//スクリプトのロード
	this->Runner = new ScriptRunner(poolMainWindow , false );
	this->Runner->LoadScript(luafilename);
	return true;

}

bool MediaFileAnalize::Analize(const std::string& dir,const std::string& filename , std::string* title,std::string* artist ,std::string* album , std::string* alias )
{
	if ( ! AnalizeLua(dir, filename , title, artist , album ,  alias) )
	{
		return false;
	}
	if ( ! AnalizeCOM(dir, filename , title, artist , album ,  alias) )
	{
		return false;
	}
	return true;
}

bool MediaFileAnalize::AnalizeLua(const std::string& dir,const std::string& filename , std::string* title,std::string* artist ,std::string* album , std::string* alias )
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


bool MediaFileAnalize::AnalizeCOM(const std::string& dir,const std::string& filename , std::string* title,std::string* artist ,std::string* album , std::string* alias )
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

