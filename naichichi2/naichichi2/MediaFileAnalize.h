#pragma once

#include <string>
#import <SHELL32.dll> rename("ShellExecute","SHELL32ShellExecute")

//ファイル情報の解析.
class MediaFileAnalize
{
public:
	MediaFileAnalize::MediaFileAnalize();
	virtual MediaFileAnalize::~MediaFileAnalize();
	xreturn::r<bool> MediaFileAnalize::Create(MainWindow* poolMainWindow,const std::string& luaCommand);

	bool MediaFileAnalize::Analize(const std::string& dir,const std::string& filename , std::string* title,std::string* artist ,std::string* album , std::string* alias ,int * part,int * rank, std::string* searchdata);

	//検索可能なデータを作る
	std::string makesearcableData(const std::string& str );
	//リストからバイグラムを生成します。
	std::string MediaFileAnalize::makeListToBigram(const std::list<std::string>& list ) const;
	//ユーザサイドが検索に使うデータを作る
	std::string MediaFileAnalize::makeUserSideSearchableData(const std::string& str ) const;
	//リストの中身を正規化します。
	std::list<std::string> MediaFileAnalize::makeListToCleanup(const std::list<std::string>& list ) const;

private:
	bool MediaFileAnalize::AnalizeLua(const std::string& dir,const std::string& filename , std::string* title,std::string* artist ,std::string* album , std::string* alias ,int * part,int * rank);
	bool MediaFileAnalize::AnalizeCOM(const std::string& dir,const std::string& filename , std::string* title,std::string* artist ,std::string* album , std::string* alias ,int * part,int * rank);

	Shell32::IShellDispatchPtr PtrShell;
	ScriptRunner* Runner;
	MainWindow* PoolMainWindow;
};
