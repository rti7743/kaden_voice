// XLConfigFile.h: XLConfigFile クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XLCONFIGFILE_H__5F9ED5B9_6207_4319_ACC1_D3625FF3F3ED__INCLUDED_)
#define AFX_XLCONFIGFILE_H__5F9ED5B9_6207_4319_ACC1_D3625FF3F3ED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class XLConfigFile  
{
public:
	XLConfigFile();
	virtual ~XLConfigFile();
public:
	/**
	 * Load:				設定ファイルを読み込む.
	 *
	 * @param inFilename	ファイル名
	 * @return bool 
	 */
	bool Load(const std::string & inFilename);

	/**
	 * Save:				書き込み
	 *
	 * @param inFilename	ファイル名
	 * @return bool 
	 */
	bool Save(const std::string & inFilename) ;


public:

	void Remove(const std::string & inKeyName ) 
	{
		std::map<std::string,std::string>::iterator i = Setting.find(inKeyName);
		if (i == Setting.end() ) return ;

		Setting.erase(i);
	}

//	static void test();
private:
	std::map<std::string,std::string>		Setting;
	bool XLConfigFile::IsComment(const char * line) const;

};

#endif // !defined(AFX_XLCONFIGFILE_H__5F9ED5B9_6207_4319_ACC1_D3625FF3F3ED__INCLUDED_)
