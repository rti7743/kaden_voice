#pragma once

#include <string>
#import <SHELL32.dll> rename("ShellExecute","SHELL32ShellExecute")

//ƒtƒ@ƒCƒ‹î•ñ‚Ì‰ğÍ.
class MediaFileAnalize
{
public:
	MediaFileAnalize::MediaFileAnalize();
	virtual MediaFileAnalize::~MediaFileAnalize();
	xreturn::r<bool> MediaFileAnalize::Create(MainWindow* poolMainWindow,const std::string& luaCommand);

	bool MediaFileAnalize::Analize(const std::string& dir,const std::string& filename , std::string* title,std::string* artist ,std::string* album , std::string* alias );


private:
	bool MediaFileAnalize::AnalizeLua(const std::string& dir,const std::string& filename , std::string* title,std::string* artist ,std::string* album , std::string* alias );
	bool MediaFileAnalize::AnalizeCOM(const std::string& dir,const std::string& filename , std::string* title,std::string* artist ,std::string* album , std::string* alias );

	Shell32::IShellDispatchPtr PtrShell;
	ScriptRunner* Runner;
};
