#include "common.h"
#include "Config.h"
#include "MainWindow.h"
#include "ScriptRunner.h"
#include "XLStringUtil.h"
#include "XLFileUtil.h"

Config::Config()
{
	IsLoad = false;
}
Config::~Config()
{
	if (this->IsLoad)
	{
		this->saveConfig(this->BaseDirectory + "\\config.conf");
	}
}

xreturn::r<std::string> Config::Create()
{
	ASSERT_IS_MAIN_THREAD_RUNNING(); //メインスレッドでしか動きません

	this->BaseDirectory = GetBaseDirectoryImpl();
	auto r = this->loadConfig(this->BaseDirectory + "\\config.conf");
	if (!r)
	{
		puts("bad config!");
		return xreturn::error("設定ファイルが読み込めません");
	}
		
	this->IsLoad = true;
	return this->BaseDirectory;
}


xreturn::r<bool> Config::loadConfig(const std::string & configFilename)
{
	boost::unique_lock<boost::mutex> al(this->lock);
	_USE_WINDOWS_ENCODING;
	this->ConfigData.clear();

	FILE * fp = fopen( XLStringUtil::pathseparator(configFilename).c_str() ,"rb");
	if (fp == NULL)
	{
		return xreturn::error("config " + configFilename + " を読み込めませんでした。");
	}
	char readbuffer[MAX_PATH];

	while( !feof(fp) )
	{
		if ( fgets(readbuffer , MAX_PATH , fp) == NULL ) break;
		char* buf = _U2A(readbuffer);

		if ( this->IsComment(buf) ) continue;

		const char * eq = strchr( buf , '=');
		if (eq == NULL) continue;

		std::string key = XLStringUtil::chop(  std::string(buf , 0 , eq - buf) ) ;
		std::string value = XLStringUtil::chop( std::string(eq + 1) ) ;

		this->ConfigData[key] = value;
	}
	fclose(fp);

	return true;
}

xreturn::r<bool> Config::saveConfig(const std::string & configFilename)
{
	boost::unique_lock<boost::mutex> al(this->lock);
	_USE_WINDOWS_ENCODING;

	std::map<std::string,std::string>	localSetting = this->ConfigData;

	std::string tmpfilename = configFilename + ".tmp";
	FILE * wfp = fopen( XLStringUtil::pathseparator(tmpfilename).c_str() ,"wb");
	if (wfp == NULL)
	{
		return xreturn::error("設定保存用の一時ファイル" + tmpfilename + "を作成できませんでした。");
	}
	FILE * fp = fopen( XLStringUtil::pathseparator(configFilename).c_str() ,"rb");
	if (fp != NULL)
	{
			char readbuffer[MAX_PATH];

			while( !feof(fp) )
			{
				if ( fgets(readbuffer , MAX_PATH , fp) == NULL ) break;
				char* buf = _U2A(readbuffer);

				if ( this->IsComment(buf) )
				{
					fprintf(wfp , "%s" , _A2U(buf));
					continue;
				}

				const char * eq = strchr( buf , '=');
				if (eq == NULL)
				{
					fprintf(wfp , "%s" , _A2U(buf));
					continue;
				}

				std::string key = XLStringUtil::chop( std::string(buf , 0 , eq - buf) ) ;
				std::string value = XLStringUtil::chop( std::string(eq + 1) ) ;

				//このデータは読み込んでいますか?
				std::map<std::string,std::string>::iterator i = localSetting.find(key);
				if (i == localSetting.end() )
				{
					//ない場合は削除
					continue;
				}
				if (value == (*i).second)
				{
					fprintf(wfp , "%s" , _A2U(buf));
					localSetting.erase(i);
					continue;
				}

				//書き換え.
				fprintf(wfp , "%s=%s\r\n" , _A2U((*i).first.c_str())  , _A2U((*i).second.c_str()) );
				localSetting.erase(i);
			}
	}
	//まだ書き込んで値は追加.
	{
		for(std::map<std::string,std::string>::iterator i = localSetting.begin() ; i != localSetting.end() ; i++)
		{
			fprintf(wfp , "%s=%s\r\n" , _A2U((*i).first.c_str())  , _A2U((*i).second.c_str()) );
		}
		fflush(wfp);
	}
	if (fp) fclose(fp);
	if (wfp) fclose(wfp);

	XLFileUtil::copy(tmpfilename,configFilename);
	XLFileUtil::del( tmpfilename );

	return true;
}



std::string Config::GetBaseDirectoryImpl() const
{
	return XLStringUtil::pathseparator(XLFileUtil::pwd() + "\\config");
}

void Config::Set(const std::string& key,const std::string& value ) 
{
	boost::unique_lock<boost::mutex> al(this->lock);
	this->ConfigData[key] = value;
}

void Config::Remove(const std::string& key ) 
{
	boost::unique_lock<boost::mutex> al(this->lock);

	auto it = this->ConfigData.find(key);

	if (it != this->ConfigData.end())
	{
		this->ConfigData.erase(it);
	}
}

std::string Config::Get(const std::string & key , const std::string & defaultValue) const
{
	boost::unique_lock<boost::mutex> al(this->lock);
	
	auto it = this->ConfigData.find(key);
	if (it == this->ConfigData.end())
	{
		return defaultValue;
	}
	return it->second;
}

double Config::GetDouble(const std::string & key , double defaultValue) const
{
	boost::unique_lock<boost::mutex> al(this->lock);
	
	auto it = this->ConfigData.find(key);
	if (it == this->ConfigData.end())
	{
		return defaultValue;
	}
	return atof(it->second.c_str());
}

int Config::GetInt(const std::string & key , int defaultValue) const
{
	boost::unique_lock<boost::mutex> al(this->lock);
	
	auto it = this->ConfigData.find(key);
	if (it == this->ConfigData.end())
	{
		return defaultValue;
	}
	return atoi(it->second.c_str());
}

bool Config::GetBool(const std::string & key , bool defaultValue) const
{
	boost::unique_lock<boost::mutex> al(this->lock);
	
	auto it = this->ConfigData.find(key);
	if (it == this->ConfigData.end())
	{
		return defaultValue;
	}
	return atoi(it->second.c_str()) != 0;
}

std::list<std::string> Config::FindGets(const std::string & prefix) const
{
	boost::unique_lock<boost::mutex> al(this->lock);
	
	std::list<std::string> r;
	for(auto it = this->ConfigData.begin() ; it != this->ConfigData.end() ; ++it )
	{
		if ( it->first.find(prefix) == 0 )
		{
			r.push_back(it->second);
		}
	}
	return r;
}

const std::map<std::string,std::string>Config::FindGetsToMap(const std::string & prefix,bool prefixtrim) const
{
	boost::unique_lock<boost::mutex> al(this->lock);
	
	std::map<std::string,std::string> r;
	for(auto it = this->ConfigData.begin() ; it != this->ConfigData.end() ; ++it )
	{
		if ( it->first.find(prefix) == 0 )
		{
			if (prefixtrim)
			{
				const std::string key = it->first.substr( prefix.size() );
				r.insert( std::pair<std::string,std::string>(key, it->second) );
			}
			else
			{
				r.insert( *it );
			}
		}
	}
	return r;
}

bool Config::ReplaceMap(const std::string& prefix,const std::map<std::string,std::string>& postData)
{
	boost::unique_lock<boost::mutex> al(this->lock);
	
	//まず該当するデータを消す
	for(auto it = this->ConfigData.begin()  , end = this->ConfigData.end();	it != end ; )
	{
		if ( it->first.find(prefix) == 0)
		{
			this->ConfigData.erase(it++);
		}
		else
		{
			++it;
		}
	}

	//もう一度データを突っ込む
	this->ConfigData.insert(postData.begin() , postData.end() );

	return true;
}



std::string Config::GetBaseDirectory() const
{
	boost::unique_lock<boost::mutex> al(this->lock);
	
	return this->BaseDirectory;
}
