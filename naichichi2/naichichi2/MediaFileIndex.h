#pragma once;
extern "C" {
	#include "../sqlite3/sqlite3.h"
};
#include "MediaFileAnalize.h"

class MediaFileIndex
{
public:
	MediaFileIndex();
	virtual ~MediaFileIndex();
	void Close();
	void MediaFileIndex::Create(MainWindow* poolMainWindow,const std::list<std::string>& mediaDirectoryListArray, const std::string& dbpath,const std::string& filenamehelperLua,const std::map<std::string,std::string>& mediaTargetExt);

	struct SearchResult 
	{
		unsigned long long		id;
		unsigned int			sizehash;
		std::string				dir;
		std::string				filename;
		std::string				title;
		std::string				artist;
		std::string				album;
		std::string				alias;
		std::string				summary;
	};
	xreturn::r< bool > MediaFileIndex::SearchQuery(std::list<MediaFileIndex::SearchResult>* result,const std::string& sqlwhere,...);
private:
	xreturn::r<bool> Open(const std::string& name);
	xreturn::r<bool> CreateTable(const std::string& name);
	
	struct filenode
	{
		filenode(unsigned long long id , unsigned int sizehash): id(id),sizehash(sizehash) , checked(false){};
		unsigned long long		id;
		unsigned int			sizehash;		//size の下位32bitのサイズ.
		bool					checked;
	};
	xreturn::r< bool > SQLFindDir(const std::string& dir,std::map<std::string,filenode>* sqlfind_dir );
	void MediaFileIndex::Scan();



	xreturn::r< unsigned long long > SQLInsertFile(const std::string& dir,const std::string& filename,unsigned int sizehash);
	xreturn::r< unsigned long long > SQLUpdateFile(unsigned long long id , const std::string& dir,const std::string& filename,unsigned int sizehash);
	xreturn::r< unsigned long long > SQLDeleteFile(unsigned long long id);
	xreturn::r<bool> ScanAndUpdate(const std::string & dir);

	bool MediaFileIndex::isTargetExt(const std::string& ext) const;

private:
	sqlite3* db;
	MainWindow* PoolMainWindow;
	std::list<std::string> mediaDirectoryListArray;
	std::map<std::string,std::string> MediaTargetExt;
	std::string dbpath;

	bool StopFlg;
	MediaFileAnalize Analize;

	boost::thread*					Thread;
};
