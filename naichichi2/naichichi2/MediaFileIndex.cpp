#include "common.h"
#include "MainWindow.h"
#include "MediaFileIndex.h"
#include "XLStringUtil.h"
#include "XLFileUtil.h"


//ファイル情報の解析.
MediaFileIndex::MediaFileIndex()
{
	this->StopFlg = false;
	this->db = NULL;
	this->Thread  = NULL;
}

MediaFileIndex::~MediaFileIndex()
{
	this->Close();
}

void MediaFileIndex::Close()
{
	this->StopFlg = true;
	if (this->Thread)
	{
		this->Thread->join();
		delete this->Thread;
		this->Thread = NULL;
	}
	if ( this->db )
	{
		sqlite3_close(this->db);
		this->db = NULL;
	}

}

void MediaFileIndex::Create(MainWindow* poolMainWindow,const std::list<std::string>& mediaDirectoryListArray, const std::string& dbpath,const std::string& filenamehelperLua,const std::map<std::string,std::string>& mediaTargetExt)
{
	assert(!this->Thread);

	this->PoolMainWindow = poolMainWindow;
	this->mediaDirectoryListArray = mediaDirectoryListArray;
	this->dbpath = dbpath;
	this->Analize.Create(poolMainWindow,filenamehelperLua);
	this->MediaTargetExt = mediaTargetExt;

	this->Open(dbpath);
	this->Thread = new boost::thread([=](){
		try
		{
			this->Scan(); 
		}
		catch(xreturn::error & e)
		{
			this->PoolMainWindow->SyncInvokeError(e.getFullErrorMessage());
		}
	});

	//スレッド優先度を最低にする.
#ifdef _WINDOWS
	SetThreadPriority( this->Thread->native_handle() , THREAD_PRIORITY_LOWEST);
#else
	int policy;
	sched_param param = {0};
	pthread_getschedparam (this->Thread->native_handle(), &policy, &param);
    policy = SCHED_RR;         // round-robin, AKA real-time scheduling
    param.sched_priority = 63; // you’ll have to play with this to see what it does
    pthread_setschedparam (this->Thread->native_handle(), policy, &param);

#endif
}

xreturn::r<bool> MediaFileIndex::Open(const std::string& name)
{
	assert(this->db == NULL);
	bool isexist = XLFileUtil::Exist(name);

	int ret = sqlite3_open(name.c_str() , &this->db);
	if (ret != SQLITE_OK)
	{
		return xreturn::error(std::string() + "データベース " + name + " のオープンに失敗しました、 sqlite3:" + sqlite3_errmsg(this->db) );
	}
	if (!isexist)
	{
		auto rr = CreateTable(name);
		if(!rr)
		{
			return xreturn::error(rr.getError());
		}
	}
	return true;
}

xreturn::r<bool> MediaFileIndex::CreateTable(const std::string& name)
{
	assert(this->db != NULL);

	int ret ;
	char create_sql[] = "CREATE VIRTUAL TABLE media  USING FTS3  ( "
                       "               id           INTEGER PRIMARY KEY"
                       "              ,sizehash     uint    NOT NULL"
                       "              ,dir          TEXT    NOT NULL"
                       "              ,filename     TEXT    NOT NULL"
                       "              ,title        TEXT    NOT NULL"
                       "              ,artist       TEXT    NOT NULL"
                       "              ,album        TEXT    NOT NULL"
                       "              ,alias        TEXT    NOT NULL"
                       "             )";
	// テーブルを生成
	char *zErrMsg = 0;
	ret = sqlite3_exec(this->db, create_sql, 0, 0, &zErrMsg);
	if (ret != SQLITE_OK)
	{
		return xreturn::error(std::string() + "データベース " + name + " のテーブル作成に失敗しました、 sqlite3:" + sqlite3_errmsg(this->db) );
	}
/*
	char createindex_sql[] = "CREATE INDEX media_index_sizehash ON media(sizehash)";
	ret = sqlite3_exec(this->db, createindex_sql, 0, 0, &zErrMsg);
	if (ret != SQLITE_OK)
	{
		return xreturn::error(std::string() + "データベース " + name + " のテーブル作成に失敗しました、 sqlite3:" + sqlite3_errmsg(this->db) );
	}
*/
	return true;
}

xreturn::r< bool > MediaFileIndex::SearchQuery(std::list<MediaFileIndex::SearchResult>* result,const std::string& sqlwhere,...)
{
	va_list ap;
	va_start(ap, result);

	std::string sql = std::string("SELECT id,sizehash,dir,filename,title,artist,album,alias FROM media WHERE ") + sqlwhere;
	char * sqlbuffer = sqlite3_mprintf( sql.c_str() ,  ap );

	va_end(ap);
	if (!sqlbuffer)
	{
		return xreturn::error("sql用メモリ確保に失敗しました.");
	}
	sqlite3_stmt* stm = NULL;
	int ret = sqlite3_prepare(this->db,sqlbuffer, -1, &stm, NULL);
	if (ret != SQLITE_OK)
	{
		std::string msg =std::string() + "sql発行に失敗しました. SQL:" + sqlbuffer + " sqlite:" + sqlite3_errmsg(this->db);
		sqlite3_free(sqlbuffer);
		return xreturn::error( msg );
	}
	for (;;) 
	{
		ret = sqlite3_step(stm);
		if (ret != SQLITE_ROW) 
		{
			break;
		}
		else
		{
			SearchResult sr;
			sr.id = sqlite3_column_int64(stm, 0);
			sr.sizehash = sqlite3_column_int(stm, 2);
			sr.dir = (const char*) sqlite3_column_text(stm, 3);
			sr.filename = (const char*) sqlite3_column_text(stm, 4);
			sr.title = (const char*) sqlite3_column_text(stm, 5);
			sr.artist = (const char*) sqlite3_column_text(stm, 6);
			sr.album = (const char*) sqlite3_column_text(stm, 7);
			sr.alias = (const char*) sqlite3_column_text(stm, 8);
//			sr.summary = this->MakeSummary();

			result->push_back(sr);
		}
	}
	sqlite3_finalize(stm);
	sqlite3_free(sqlbuffer);
	return true;
}

void MediaFileIndex::Scan()
{
	while(!this->StopFlg)
	{
		//ディレクトリスキャン
		auto it = this->mediaDirectoryListArray.begin();
		for(;it!=this->mediaDirectoryListArray.end();++it)
		{
			if(this->StopFlg)
			{
				break;
			}
			ScanAndUpdate(*it);
		}
		//スキャンが終わったので待機する
		
	}
}

xreturn::r< bool > MediaFileIndex::SQLFindDir(const std::string& dir,std::map<std::string,filenode>* sqlfind_dir )
{
	char * sqlbuffer = sqlite3_mprintf("SELECT id,filename,sizehash FROM media WHERE dir = %Q" , dir.c_str() );
	if (!sqlbuffer)
	{
		return xreturn::error("sql用メモリ確保に失敗しました.");
	}

	sqlite3_stmt* stm = NULL;
	int ret = sqlite3_prepare(this->db,sqlbuffer, -1, &stm, NULL);
	if (ret != SQLITE_OK)
	{
		std::string msg =std::string() + "sql発行に失敗しました. SQL:" + sqlbuffer + " sqlite:" + sqlite3_errmsg(this->db);
		sqlite3_free(sqlbuffer);
		return xreturn::error( msg );
	}
	for (;;) 
	{
		ret = sqlite3_step(stm);
		if (ret != SQLITE_ROW) 
		{
			break;
		}
		else
		{
			unsigned long long id = sqlite3_column_int64(stm, 0);
			const char* filename = (const char*) sqlite3_column_text(stm, 1); //filename
			unsigned int sizehash = sqlite3_column_int(stm, 2); //sizehash

			sqlfind_dir->insert(std::pair<std::string,filenode>( filename , filenode(id,sizehash)) );
		}
	}
	sqlite3_finalize(stm);
	sqlite3_free(sqlbuffer);
	return true;
}



xreturn::r< unsigned long long > MediaFileIndex::SQLInsertFile(const std::string& dir,const std::string& filename,unsigned int sizehash)
{
	//ファイルを解析.
	std::string title;
	std::string artist;
	std::string album;
	std::string alias;
	this->Analize.Analize(dir,filename,&title,&artist,&album,&alias);

	char * sqlbuffer = sqlite3_mprintf("INSERT INTO media(dir, filename,sizehash,title,artist,album,alias) VALUES (%Q,%Q,%u,%Q,%Q,%Q,%Q)"
			, dir.c_str() ,filename.c_str() ,sizehash ,title.c_str() ,artist.c_str() ,album.c_str() ,alias.c_str() );
	if (!sqlbuffer)
	{
		return xreturn::error("sql用メモリ確保に失敗しました." );
	}

	sqlite3_stmt* stm = NULL;
	int ret = sqlite3_prepare(this->db,sqlbuffer, -1, &stm, NULL);
	if (ret != SQLITE_OK)
	{
		std::string msg =std::string() + "sql構文構築に失敗しました. SQL:" + sqlbuffer + " sqlite:" + sqlite3_errmsg(this->db);
		sqlite3_free(sqlbuffer);
		return xreturn::error( msg );
	}
	ret = sqlite3_step(stm);
	if (ret != SQLITE_DONE)
	{
		std::string msg =std::string() + "sql発行に失敗しました. SQL:" + sqlbuffer + " sqlite:" + sqlite3_errmsg(this->db);
		sqlite3_free(sqlbuffer);
		return xreturn::error( msg );
	}
	unsigned long long id = sqlite3_last_insert_rowid(this->db);
	sqlite3_finalize(stm);
	sqlite3_free(sqlbuffer);
	return id;
}

xreturn::r< unsigned long long > MediaFileIndex::SQLUpdateFile(unsigned long long id , const std::string& dir,const std::string& filename,unsigned int sizehash)
{
	//ファイルを解析.
	std::string title;
	std::string artist;
	std::string album;
	std::string alias;
	this->Analize.Analize(dir,filename,&title,&artist,&album,&alias);

	char * sqlbuffer = sqlite3_mprintf("UPDATE FROM media SET dir=%Q, filename=%Q ,sizehash=%u ,title=%Q ,artist=%Q ,album=%Q ,alias=%Q where id=%u"
			, dir.c_str() ,filename.c_str() ,sizehash ,title.c_str() ,artist.c_str() ,album.c_str() ,alias.c_str(),id );
	if (!sqlbuffer)
	{
		return xreturn::error("sql用メモリ確保に失敗しました." );
	}

	sqlite3_stmt* stm = NULL;
	int ret = sqlite3_prepare(this->db,sqlbuffer, -1, &stm, NULL);
	if (ret != SQLITE_OK)
	{
		std::string msg =std::string() + "sql構文構築に失敗しました. SQL:" + sqlbuffer + " sqlite:" + sqlite3_errmsg(this->db);
		sqlite3_free(sqlbuffer);
		return xreturn::error( msg );
	}
	ret = sqlite3_step(stm);
	if (ret != SQLITE_DONE)
	{
		std::string msg =std::string() + "sql発行に失敗しました. SQL:" + sqlbuffer + " sqlite:" + sqlite3_errmsg(this->db);
		sqlite3_free(sqlbuffer);
		return xreturn::error( msg );
	}
	sqlite3_finalize(stm);
	sqlite3_free(sqlbuffer);
	return id;
}

xreturn::r< unsigned long long > MediaFileIndex::SQLDeleteFile(unsigned long long id)
{
	char * sqlbuffer = sqlite3_mprintf("DELETE FROM media where id=%u" , id );
	if (!sqlbuffer)
	{
		return xreturn::error("sql用メモリ確保に失敗しました." );
	}

	sqlite3_stmt* stm = NULL;
	int ret = sqlite3_prepare(this->db,sqlbuffer, -1, &stm, NULL);
	if (ret != SQLITE_OK)
	{
		std::string msg =std::string() + "sql構文構築に失敗しました. SQL:" + sqlbuffer + " sqlite:" + sqlite3_errmsg(this->db);
		sqlite3_free(sqlbuffer);
		return xreturn::error( msg );
	}
	ret = sqlite3_step(stm);
	if (ret != SQLITE_DONE)
	{
		std::string msg =std::string() + "sql発行に失敗しました. SQL:" + sqlbuffer + " sqlite:" + sqlite3_errmsg(this->db);
		sqlite3_free(sqlbuffer);
		return xreturn::error( msg );
	}
	sqlite3_finalize(stm);
	sqlite3_free(sqlbuffer);
	return id;
}

xreturn::r<bool> MediaFileIndex::ScanAndUpdate(const std::string & dir)
{
	//SQLLiteに格納されているディレクトリ情報を取得します。
	std::map<std::string,filenode> sqlfind_dir;
	{
		auto r = SQLFindDir(dir,&sqlfind_dir );
		if (!r)
		{
			return xreturn::error(r.getError());
		}
	}

	//ディレクトリに格納されている情報を取得してマッチングしていきます。
	std::string path = dir + "\\*.*";
	WIN32_FIND_DATA data;
	HANDLE handle = ::FindFirstFile( path.c_str() , &data);
	if (handle == INVALID_HANDLE_VALUE)
	{
		return true;
	}
	do
	{
		if (this->StopFlg)
		{
			//スレッド終了フラグ
			return false;
		}

		if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if ( data.cFileName[0] == '.' && (data.cFileName[1] == '\0' || (data.cFileName[1] == '.' && data.cFileName[2] == '\0') ) )
			{
				continue;
			}
			ScanAndUpdate(dir + "\\" + data.cFileName);
			continue;
		}
		if (! isTargetExt(XLStringUtil::strtolower(XLStringUtil::baseext( data.cFileName ) ) )  )
		{//対象外の拡張子
			continue;
		}

		auto it = sqlfind_dir.find(data.cFileName);
		if (it == sqlfind_dir.end() )
		{//ねーよ
			auto r = SQLInsertFile(dir,std::string(data.cFileName),data.nFileSizeLow);
			if (!r)
			{
				return xreturn::error(r.getError());
			}
		}
		else if (it->second.sizehash != data.nFileSizeLow)
		{//あるけど更新されている!
			auto r = SQLUpdateFile(it->second.id,dir, std::string(data.cFileName) ,data.nFileSizeLow);
			if (!r)
			{
				return xreturn::error(r.getError());
			}
			it->second.checked = true;
		}
		else
		{//変更なし.
			it->second.checked = true;
			continue;
		}
	}
	while( ::FindNextFile( handle , &data) );
	::FindClose(handle);

	//削除されたファイルの情報を消す.
	for(auto it = sqlfind_dir.begin() ; it != sqlfind_dir.end() ; ++it)
	{
		if (!it->second.checked)
		{
			SQLDeleteFile(it->second.id);
		}
	}

	return true;
}

bool MediaFileIndex::isTargetExt(const std::string& ext) const
{
	return  this->MediaTargetExt.find(ext) != this->MediaTargetExt.end();
}