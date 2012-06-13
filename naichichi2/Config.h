#pragma once

class Config
{
public:
	Config();
	virtual ~Config();
	xreturn::r<std::string> Create();

	void Set(const std::string& key,const std::string& value ) ;
	std::string Get(const std::string & key , const std::string & defaultValue) const;
	double GetDouble(const std::string & key , double defaultValue) const;
	int GetInt(const std::string & key , int defaultValue) const;
	bool GetBool(const std::string & key , bool defaultValue) const;
	std::list<std::string> FindGets(const std::string & prefix) const;
	const std::map<std::string,std::string>FindGetsToMap(const std::string & prefix) const;
	std::string GetBaseDirectory() const;

	xreturn::r<bool> loadConfig(const std::string & configFilename);
	xreturn::r<bool> saveConfig(const std::string & configFilename);
private:
	bool IsComment(const char * line) const
	{
		return ( line[0] == '\0' ||  line[0] == '#' || line[0] == ';' || line[0] == '\'' || (line[0] == '/' &&  line[1] == '/') || (line[0] == '-' &&  line[1] == '-') );
	}
	std::string GetBaseDirectoryImpl() const;
	
	
	std::map<std::string , std::string > ConfigData;
	std::string BaseDirectory;	//\\は含まない!
	bool IsLoad;

	mutable boost::mutex lock;
};