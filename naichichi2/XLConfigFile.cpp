// XLConfigFile.cpp: XLConfigFile クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////
#include <string>
#include <map>
#include "XLConfigFile.h"
#include "XLStringUtil.h"

//USE_TEST(XLConfigFile)


//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

XLConfigFile::XLConfigFile()
{

}

XLConfigFile::~XLConfigFile()
{

}

bool XLConfigFile::IsComment(const char * line) const
{
	return ( line[0] == '\0' ||  line[0] == '#' || line[0] == ';' || line[0] == '\'' || (line[0] = '/' &&  line[1] = '/') || (line[0] = '-' &&  line[1] = '-') );
}

/**
 * Load:				設定ファイルを読み込む.
 *
 * @param inFilename	ファイル名
 * @return bool 
 */
bool XLConfigFile::Load(const std::string & inFilename)
{
	FILE * fp = fopen( inFilename.c_str() ,"rb");
	if (fp == NULL)
	{
		return false;
	}
	char buf[MAX_PATH];

	while( !feof(fp) )
	{
		if ( fgets(buf , MAX_PATH , fp) == NULL ) break;

		if ( this->IsComment(buf) ) continue;

		const char * eq = strchr( buf , '=');
		if (eq == NULL) continue;

		std::string key = XLStringUtil::chop(  std::string(buf , 0 , eq - buf) ) ;
		std::string value = XLStringUtil::chop( std::string(eq + 1) ) ;

		Setting[key] = value;
	}
	fclose(fp);

	return true;
}

/**
 * Save:				書き込み
 *
 * @param inFilename	ファイル名
 * @return bool 
 */
bool XLConfigFile::Save(const std::string & inFilename)
{
	XLStringMap	localSetting = Setting;

	std::string tmpfilename = inFilename + ".tmp";
	FILE * wfp = fopen( tmpfilename.c_str() ,"wb");
	if (wfp == NULL)
	{
		return false;
	}
	FILE * fp = fopen( inFilename.c_str() ,"rb");
	if (fp != NULL)
	{
			char buf[MAX_PATH];

			while( !feof(fp) )
			{
				if ( fgets(buf , MAX_PATH , fp) == NULL ) break;

				if ( this->IsComment(buf) )
				{
					fprintf(wfp , "%s" , buf);
					continue;
				}

				const char * eq = strchr( buf , '=');
				if (eq == NULL)
				{
					fprintf(wfp , "%s" , buf);
					continue;
				}

				std::string key = XLStringUtil::chop( std::string(buf , 0 , eq - buf) ) ;
				std::string value = XLStringUtil::chop( std::string(eq + 1) ) ;

				//このデータは読み込んでいますか?
				XLStringMap::iterator i = localSetting.find(key);
				if (i == localSetting.end() )
				{
					//ない場合は削除
					continue;
				}
				if (value == (*i).second)
				{
					fprintf(wfp , "%s" , buf);
					localSetting.erase(i);
					continue;
				}

				//書き換え.
				fprintf(wfp , "%s=%s\r\n" , (*i).first.c_str()  , (*i).second.c_str() );

				localSetting.erase(i);
			}
	}
	//まだ書き込んで値は追加.
	{
		for(XLStringMap::iterator i = localSetting.begin() ; i != localSetting.end() ; i++)
		{
			fprintf(wfp , "%s=%s\r\n" , (*i).first.c_str()  , (*i).second.c_str() );
		}
		fflush(wfp);
	}
	if (fp) fclose(fp);
	if (wfp) fclose(wfp);

	CopyFile( tmpfilename.c_str() , inFilename.c_str() , FALSE);
	unlink( tmpfilename.c_str() );

	return true;
}

/*
void XLConfigFile::test()
{
	//書き換えテスト
	{
		XLConfigFile	rcf;
		rcf.Load("test/conftest/test2.txt");

		std::string ret;
		ASSERT( (ret = rcf.Setting["abc"]) == "10");
		ASSERT( (ret = rcf.Setting["xyz"]) == "20");
		ASSERT( (ret = rcf.Setting["sister"]) == "kaho");
		ASSERT( (ret = rcf.Setting["meid"]) == "mahoro");

		rcf.Setting["meid"] = "minawa";
		rcf.Save("test/conftest/test2.txt");

		rcf.Load("test/conftest/test2.txt");
		ASSERT( (ret = rcf.Setting["abc"]) == "10");
		ASSERT( (ret = rcf.Setting["xyz"]) == "20");
		ASSERT( (ret = rcf.Setting["sister"]) == "kaho");
		ASSERT( (ret = rcf.Setting["meid"]) == "minawa");

		rcf.Setting["meid"] = "mahoro";
		rcf.Save("test/conftest/test2.txt");
	}
	//ロードテスト
	{
		XLConfigFile	rcf;
		rcf.Load("test/conftest/test1.txt");

		std::string ret;
		ASSERT( (ret = rcf.Setting["abc"]) == "10");
		ASSERT( (ret = rcf.Setting["xyz"]) == "20");
		ASSERT( (ret = rcf.Setting["sister"]) == "kaho");
		ASSERT( (ret = rcf.Setting["meid"]) == "mahoro");
	}
}
*/