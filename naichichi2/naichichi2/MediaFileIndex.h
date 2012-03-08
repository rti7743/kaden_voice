#pragma once;
extern "C" {
	#include "../sqlite3/sqlite3.h"
};
#include "MediaFileAnalize.h"
#include "MediaFileImageShot.h"

class MediaFileIndex
{
public:
	MediaFileIndex();
	virtual ~MediaFileIndex();
	void Close();
	void MediaFileIndex::Create(MainWindow* poolMainWindow,const std::list<std::string>& mediaDirectoryListArray, const std::string& dbpath,const std::string& filenamehelperLua,const std::map<std::string,std::string>& mediaTargetExt,const std::map<std::string,std::string>& mediaDefualtIcon);

	struct SearchResult 
	{//UTF8な文字列ポインタになります。(開放の義務なし)
		unsigned long long		id;
		unsigned int			sizehash;
		time_t					accesstime;
		int						rank;
		const char*				type;
		const char*				dir;
		const char*				filename;
		const char*				title;
		const char*				artist;
		const char*				album;
		const char*				alias;
		unsigned int			part;
		const char*				image;	//base64
		const char*				summary;
	};
	xreturn::r< bool > MediaFileIndex::SearchQuery(const std::string& query,std::function<bool(const MediaFileIndex::SearchResult& sr)> callback) const;
	xreturn::r< bool > MediaFileIndex::UpdateMedia(const std::string& type) const;
private:
	xreturn::r<bool> Open(const std::string& name);
	xreturn::r<bool> CreateTable(const std::string& name);
	std::string MediaFileIndex::ImageToBase64(const std::map<std::string,std::string>& mediaDefualtIcon,const std::string& key) const;
	
	struct filenode
	{
		filenode(unsigned long sizehash):sizehash(sizehash) , checked(false){};
		unsigned long			sizehash;		//size の下位32bitのサイズ.
		bool					checked;
	};
	xreturn::r< bool > SQLFindDir(const std::string& dir,std::map<std::string,filenode>* sqlfind_dir );
	void MediaFileIndex::Scan();



	xreturn::r<bool > SQLInsertFile(const std::string& dir,const std::string& filename,unsigned long sizehash,time_t accesstime , const std::string& type);
	xreturn::r<bool > SQLUpdateFile(const std::string& dir,const std::string& filename,unsigned long sizehash,time_t accesstime , const std::string& type);
	xreturn::r<bool > SQLDeleteFile(const std::string& dir,const std::string& filename);
	xreturn::r<bool> ScanAndUpdate(const std::string & dir);

	//拡張子から種類を返す
	std::string MediaFileIndex::GetExtType(const std::string& filename) const;
	//ディフォルトのアイコンを取得します。
	std::string GetDefaultIcon(const std::string& type) const;
	//sql escape (prepare bind が最高なのはわかるけど、複雑なSQL組立だと難しいよ・・・ PHPだともう少しうまく書けるフレームワークがあるけども。)
	std::string MediaFileIndex::SQLAppend(const char * sql , ...) const;
	//リストに音声認識で使うデータを追加します。
	void MediaFileIndex::AppendConvertRecognitionList(std::list<std::string> * list , const std::string& text) const;

private:
	sqlite3* db;
	MainWindow* PoolMainWindow;
	std::list<std::string> mediaDirectoryListArray;
	std::map<std::string,std::string> MediaTargetExt;
	std::string dbpath;

	//個別の画像がないアプリケーションはナイチチ側で用意するアイコンを使う。
	//media.dbにアイコン画像も入れると容量の無駄なので・・・
	std::string shareIconBase64Music;
	std::string shareIconBase64Video;
	std::string shareIconBase64Book;

	bool StopFlg;
	MediaFileAnalize Analize;
	MediaFileImageShot ImageShot;

	boost::thread*					Thread;
};
