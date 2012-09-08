#pragma once

#include <vector>

class XLHttpRequerst
{
public:
	
	bool Download(const std::string & url , const std::map<std::string,std::string> & header,const std::map<std::string,std::string> & option);
	std::string GetContents(const std::string & url , const std::map<std::string,std::string> & header,const std::map<std::string,std::string> & option)
	{
		this->Download(url ,  header, option);
		return this->getDataString();
	}

	///////////////////////////////////////////////////////////////////
	//結果をどうするかの処理
	///////////////////////////////////////////////////////////////////

	const unsigned char* getData() const
	{
		return &this->OutData[0];
	}
	const unsigned int getSize() const
	{
		return this->OutData.size();
	}
	const std::string getDataString() const
	{
		return std::string((char*) (&this->OutData[0]));
	}
	bool saveFile(const std::string & filename) const;
	
private:
	//ダウンロードしたデータ
	std::vector<unsigned char> OutData;
	//steamんかて使いづらいから大嫌いだ、ばーか。
	//std::basic_stringstream<unsigned char> OutData;
};