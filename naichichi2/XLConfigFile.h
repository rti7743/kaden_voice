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
	//基本形.
	template<typename _T>
	_T LoadOrDefault(const std::string & inKeyName ,_T inDefualt  ) const
	{
		std::map<std::string,std::string>::const_iterator i = Setting.find(inKeyName);
		if (i == Setting.end() ) return inDefualt;
		return atoi( (*i).second.c_str() );
	}

	//string の場合.
	template<>
	std::string LoadOrDefault<std::string>(const std::string & inKeyName ,const std::string & inDefualt  ) const
	{
		std::map<std::string,std::string>::const_iterator i = Setting.find(inKeyName);
		if (i == Setting.end() )
		{
			return inDefualt;
		}
		return (*i).second;
	}
	//bool
	template<>
	bool LoadOrDefault<std::string>(const std::string & inKeyName ,bool inDefualt  ) const
	{
		std::map<std::string,std::string>::const_iterator i = Setting.find(inKeyName);
		if (i == Setting.end() ) return inDefualt;
		return atoi( (*i).second.c_str() ) == 0 ? false : true;
	}
	//double
	template<>
	double LoadOrDefault<std::string>(const std::string & inKeyName ,double inDefualt  ) const
	{
		std::map<std::string,std::string>::const_iterator i = Setting.find(inKeyName);
		if (i == Setting.end() ) return inDefualt;
		return atof( (*i).second.c_str() );
	}

	//基本形.
	template <typename _T>
	void Upsert(const std::string & inKeyName ,_T inValue  ) 
	{
		Setting[inKeyName] = num2str(inValue);
	}
	template<>
	void Upsert<std::string>(const std::string & inKeyName ,string  inValue)
	{
		Setting[inKeyName] = inValue;
	}
	template<>
	void Upsert<bool>(const std::string & inKeyName ,bool  inValue)
	{
		Setting[inKeyName] = num2str(inValue == true ? 1 : 0);
	}
	template<>
	void Upsert<bool>(const std::string & inKeyName ,double  inValue)
	{
		Setting[inKeyName] = format("%f", inValue);
	}

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
