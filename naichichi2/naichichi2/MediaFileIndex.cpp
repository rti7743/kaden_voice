#include "common.h"
#include <io.h>
#include "MainWindow.h"
#include "MediaFileIndex.h"
#include "XLStringUtil.h"
#include "XLFileUtil.h"

const static std::string g_Base64BadIcon = "/9j/4AAQSkZJRgABAQEAYABgAAD/2wBDAAYEBQYFBAYGBQYHBwYIChAKCgkJChQODwwQFxQYGBcUFhYaHSUfGhsjHBYWICwgIyYnKSopGR8tMC0oMCUoKSj/2wBDAQcHBwoIChMKChMoGhYaKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCgoKCj/wAARCABGAEMDASIAAhEBAxEB/8QAGgABAQEBAQEBAAAAAAAAAAAAAAQDAgUBCP/EADEQAAICAQMDAgQCCwAAAAAAAAECAAMRBBIhEzFRBUEUImFxgZEjMjQ1QlNic7LB8P/EABQBAQAAAAAAAAAAAAAAAAAAAAD/xAAUEQEAAAAAAAAAAAAAAAAAAAAA/w==";

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

std::string MediaFileIndex::ImageToBase64(const std::map<std::string,std::string>& mediaDefualtIcon,const std::string& key) const
{
	auto it = mediaDefualtIcon.find(key);
	if (it == mediaDefualtIcon.end() )
	{
		this->PoolMainWindow->SyncInvokeLog("ディフォルトアイコンタイプ " + key + "の設定がありません",LOG_LEVEL_ERROR);
		return g_Base64BadIcon;
	}
	const std::string imagefilename = this->PoolMainWindow->Config.GetBaseDirectory() + "\\" + it->second ;

	std::vector<char> buffer;
	if ( ! XLFileUtil::cat_b(imagefilename,&buffer) )
	{
		this->PoolMainWindow->SyncInvokeLog("ディフォルトアイコン " + imagefilename + "をロードできませんでした",LOG_LEVEL_ERROR);
		return g_Base64BadIcon;
	}
	return XLStringUtil::base64encode(&buffer[0],buffer.size());
}

void MediaFileIndex::Create(MainWindow* poolMainWindow,const std::list<std::string>& mediaDirectoryListArray, const std::string& dbpath,const std::string& filenamehelperLua,const std::map<std::string,std::string>& mediaTargetExt,const std::map<std::string,std::string>& mediaDefualtIcon)
{
	assert(!this->Thread);

	this->PoolMainWindow = poolMainWindow;
	this->mediaDirectoryListArray = mediaDirectoryListArray;
	this->dbpath = dbpath;
	this->Analize.Create(poolMainWindow,filenamehelperLua);
	this->MediaTargetExt = mediaTargetExt;

	//ディフォルトのアイコンをロードして base64化する.
	this->shareIconBase64Music = this->ImageToBase64(mediaDefualtIcon,"music");
	this->shareIconBase64Video = this->ImageToBase64(mediaDefualtIcon,"video");
	this->shareIconBase64Book = this->ImageToBase64(mediaDefualtIcon,"book");

	this->Open(dbpath);
	this->Thread = new boost::thread([=](){
		try
		{
#ifdef _WINDOWS
			COMInit cominit;
//			GdiPlusInit gdiPlusInit;
#endif
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
	char create_sql[] = "CREATE VIRTUAL TABLE media  USING FTS4  ( "
//                       "               id           INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL" //fts3では auto inclimenが効かないらしい。
                                                                                                  //ユニーク判定は dir と filename の組み合わせで行う。
                       "               sizehash     uint    NOT NULL"
                       "              ,accesstime   uint    NOT NULL"
                       "              ,rank         uint    NOT NULL"
                       "              ,type         TEXT    NOT NULL" //種別データ
                       "              ,dir          TEXT    NOT NULL"
                       "              ,filename     TEXT    NOT NULL"
                       "              ,title        TEXT    NOT NULL"
                       "              ,artist       TEXT    NOT NULL"
                       "              ,album        TEXT    NOT NULL"
                       "              ,alias        TEXT    NOT NULL"
                       "              ,part         int     NOT NULL" //第N話
                       "              ,searchdata   TEXT    NOT NULL" //検索に使うデータ
                       "              ,image        BLOB    NOT NULL"
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

xreturn::r< bool > MediaFileIndex::SearchQuery(const std::string& query,unsigned int from,unsigned int to
	,const std::string& type,std::function<bool(const MediaFileIndex::SearchResult& sr)> callback) const
{
	_USE_WINDOWS_ENCODING;

	//全角スペースを半角に
	std::string q = XLStringUtil::replace(query , "　", " ");

	//コマンドパース
	std::list<std::string> wordsArray;
	auto qlist = XLStringUtil::split(" ",q);
	for(auto it = qlist.begin() ; it != qlist.end() ; ++it)
	{
		{//それ以外は検索ワード
			if (!it->empty())
			{//sqlite検索のために 綺麗な検索データを作る.
				std::string qq = XLStringUtil::chop(*it);
				qq = XLStringUtil::mb_convert_kana(qq,"Hca"); //半角カナ、カタカナはひらがなに直します。辞書はひらがなで作っています。
				qq = XLStringUtil::strtolower(qq);            //全部小文字に
				wordsArray.push_back(qq);
			}
		}
	}
	std::string sql = "SELECT sizehash,accesstime,rank,type,dir,filename,title,artist,album,alias,image FROM media ";
	std::list<std::string> whereList;
	if ( type == "music" )
	{
		whereList.push_back("type = 'music'");
	}
	else if ( type == "video" )
	{
		whereList.push_back("type = 'video'");
	}
	else if ( type == "book" )
	{
		whereList.push_back("type = 'book'");
	}

	if (! wordsArray.empty() )
	{
		std::string searchBigram = this->Analize.makeListToBigram(wordsArray);
		whereList.push_back( SQLAppend("searchdata MATCH (%Q)",  searchBigram.c_str()) );
	}

	if (! whereList.empty())
	{
		sql +=  "WHERE " + XLStringUtil::join(" AND " , whereList);
	}

	if (from > to) std::swap(from,to);
	sql += SQLAppend(" limit %d OFFSET %d",  to - from,from );

	this->PoolMainWindow->SyncInvokeLog(std::string() + "検索sql発行:" + sql ,LOG_LEVEL_DEBUG);

	sqlite3_stmt* stm = NULL;
	int ret = sqlite3_prepare_v2(this->db,_A2U(sql.c_str()), -1, &stm, NULL);
	if (ret != SQLITE_OK)
	{
		std::string msg =std::string() + "sql発行に失敗しました. sqlite:" + sqlite3_errmsg(this->db) + " SQL:" + sql ;
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
//			sr.id = sqlite3_column_int64(stm, 0);
			sr.sizehash = sqlite3_column_int(stm, 0);
			sr.accesstime = sqlite3_column_int64(stm, 1);
			sr.rank = sqlite3_column_int(stm, 2);
			sr.type = (const char*) sqlite3_column_text(stm, 3);
			sr.dir = (const char*) sqlite3_column_text(stm, 4);
			sr.filename = (const char*) sqlite3_column_text(stm, 5);
			sr.title = (const char*) sqlite3_column_text(stm, 6);
			sr.artist = (const char*) sqlite3_column_text(stm, 7);
			sr.album = (const char*) sqlite3_column_text(stm, 8);
			sr.alias = (const char*) sqlite3_column_text(stm, 9);
			const void* image = sqlite3_column_blob(stm, 10);
			int imagesize = sqlite3_column_bytes(stm, 10);

			std::string imageBase64String;
			if (imagesize > 0)
			{
				imageBase64String = XLStringUtil::base64encode((const char*)image,imagesize);
			}
			else
			{
				imageBase64String = this->GetDefaultIcon(sr.type);
			}
			sr.image = imageBase64String.c_str();
			sr.summary = "";
//			sr.summary = this->MakeSummary();

			if (! callback(sr) )
			{
				break;
			}
		}
	}
	sqlite3_finalize(stm);
	this->PoolMainWindow->SyncInvokeLog(std::string() + "find done." ,LOG_LEVEL_DEBUG);
	return true;
}

xreturn::r< bool > MediaFileIndex::UpdateMedia(const std::string& type) const
{
	return true;
	_USE_WINDOWS_ENCODING;

	//アルゴリズム
	//rank,accesstime でソートし、上位500件取得する。
	//その中のファイル名、タイトル、アーティスト、アルバム、エイリアスをひらがな化して、記号などをスペース化する。
	//スペース分割して、使えそうなものを拾ってリストに入れる。
	//リストから重複を除いたものを音声認識キーとして採用する。
	std::string sql = SQLAppend("SELECT filename,title,artist,album,alias FROM media where type = %Q order by rank desc,accesstime desc limit 500", type.c_str());

	sqlite3_stmt* stm = NULL;
	int ret = sqlite3_prepare_v2(this->db,sql.c_str(), -1, &stm, NULL);
	if (ret != SQLITE_OK)
	{
		std::string msg =std::string() + "sql発行に失敗しました. SQL:" + sql + " sqlite:" + sqlite3_errmsg(this->db);
		return xreturn::error( msg );
	}

	std::list<std::string> recognitionList;
	for (;;) 
	{
		ret = sqlite3_step(stm);
		if (ret != SQLITE_ROW) 
		{
			break;
		}
		else
		{
			if (this->StopFlg)
			{
				break;
			}
			const char* filenameUTF8 = (const char*) sqlite3_column_text(stm, 0);
			const char* titleUTF8 = (const char*) sqlite3_column_text(stm, 1);
			const char* artistUTF8 = (const char*) sqlite3_column_text(stm, 2);
			const char* albumUTF8 = (const char*) sqlite3_column_text(stm, 3);
			const char* aliasUTF8 = (const char*) sqlite3_column_text(stm, 4);

//			AppendConvertRecognitionList(&recognitionList,XLStringUtil::basenameonly(_U2A(filenameUTF8)));
			AppendConvertRecognitionList(&recognitionList,_U2A(titleUTF8)); //今はタイトルだけにするよ
//			AppendConvertRecognitionList(&recognitionList,_U2A(artistUTF8));
//			AppendConvertRecognitionList(&recognitionList,_U2A(albumUTF8));
//			AppendConvertRecognitionList(&recognitionList,_U2A(aliasUTF8));
		}
	}
	sqlite3_finalize(stm);
	if (this->StopFlg)
	{
		return true;
	}

	//重複を削除.
	recognitionList = XLStringUtil::unique(recognitionList);

	//音声認識エンジンに登録する.
	//音声認識エンジンには、メインスレッド経由でアクセスするよ。
	this->PoolMainWindow->SyncInvoke( [&](){
		try
		{
			this->PoolMainWindow->Recognition.UpdateMedia(type,recognitionList); 
		}
		catch(xreturn::error & e)
		{
			this->PoolMainWindow->SyncInvokeError(e.getFullErrorMessage());
		}
	} );

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
				return ;
			}
			ScanAndUpdate(*it);
		}
		this->PoolMainWindow->SyncInvokeLog(std::string() + "ディレクトリスキャンが終わりました");
		if(this->StopFlg)
		{
			return ;
		}

		//音声認識をアップデート.
		this->UpdateMedia("music");
		if(this->StopFlg)
		{
			return ;
		}
		this->UpdateMedia("video");
		if(this->StopFlg)
		{
			return ;
		}
		this->UpdateMedia("book");
		if(this->StopFlg)
		{
			return ;
		}
		this->PoolMainWindow->SyncInvoke( [&](){
			try
			{
				this->PoolMainWindow->Recognition.CommitRule(); 
			}
			catch(xreturn::error & e)
			{
				this->PoolMainWindow->SyncInvokeError(e.getFullErrorMessage());
			}
		} );
		this->PoolMainWindow->SyncInvokeLog(std::string() + "音声正規表現特殊ルールアップデートが完了しました");

		//消えてしまったディレクトリや、除外設定されたディレクトリの検索データを消去します。
		ScanAndDeleteLostDirectory();
		if(this->StopFlg)
		{
			return ;
		}

		//スキャンが終わったので待機する
		break;

	}
}

xreturn::r< bool > MediaFileIndex::SQLFindDir(const std::string& dir,std::map<std::string,filenode>* sqlfind_dir )
{
	_USE_WINDOWS_ENCODING;

	char * sqlbuffer = sqlite3_mprintf("SELECT filename,sizehash FROM media WHERE dir = %Q" , _A2U(dir.c_str()) );
	if (!sqlbuffer)
	{
		return xreturn::error("sql用メモリ確保に失敗しました.");
	}

	sqlite3_stmt* stm = NULL;
	int ret = sqlite3_prepare_v2(this->db,sqlbuffer, -1, &stm, NULL);
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
			const char* filenameUTF8 = (const char*) sqlite3_column_text(stm, 0); //filename
			unsigned long sizehash = sqlite3_column_int(stm, 1); //sizehash

			sqlfind_dir->insert(std::pair<std::string,filenode>( _U2A(filenameUTF8) , filenode(sizehash)) );
		}
	}
	sqlite3_finalize(stm);
	sqlite3_free(sqlbuffer);
	return true;
}



xreturn::r<bool > MediaFileIndex::SQLInsertFile(const std::string& dir,const std::string& filename,unsigned long sizehash,time_t accesstime , const std::string& type)
{
	_USE_WINDOWS_ENCODING;

	//ファイルを解析.
	std::string title;
	std::string artist;
	std::string album;
	std::string alias;
	int rank;
	int part;
	std::string searchdata;
	auto r1 = this->Analize.Analize(dir,filename,&title,&artist,&album,&alias,&part,&rank,&searchdata);
	if (!r1)
	{
		this->PoolMainWindow->SyncInvokeLog(std::string() + "ファイル解析中にエラーが発生しました。スキップします。 エラー:" + r1.getErrorMessage() , LOG_LEVEL_ERROR);
	}

	//一覧に出すために、ファイルのスクリーンショットを取得
	std::vector<char> pngImage;
	auto r2 = this->ImageShot.Shot(dir,filename,&pngImage);
	if (!r2)
	{
		this->PoolMainWindow->SyncInvokeLog(std::string() + "サムネイル撮影中にエラーが発生しました。スキップします。 エラー:" + r2.getErrorMessage() , LOG_LEVEL_ERROR);
		pngImage.clear();
	}
	this->PoolMainWindow->SyncInvokeLog(std::string() + "index追加:"+dir+"\\"+filename);

	//UTF8に変換
	std::string dirUTF8 = _A2U(dir.c_str());
	std::string filenameUTF8 = _A2U(filename.c_str());
	std::string typeUTF8 = _A2U(type.c_str());
	std::string titleUTF8 = _A2U(title.c_str());
	std::string artistUTF8 = _A2U(artist.c_str());
	std::string albumUTF8 = _A2U(album.c_str());
	std::string aliasUTF8 = _A2U(alias.c_str());
	std::string searchdataUTF8 = _A2U(searchdata.c_str());
	if (!pngImage.empty()){
		rank++;
	}

  	//blobがあるからsqlite3_mprintf ではなくて、 bindする
	const char * sql = "INSERT INTO media(sizehash,accesstime,rank,type,title,artist,album,alias,part,searchdata,image,dir, filename) VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?)";

	sqlite3_stmt* stm = NULL;
	int ret = sqlite3_prepare_v2(this->db,sql, -1, &stm, NULL);
	if (ret != SQLITE_OK)
	{
		std::string msg =std::string() + "sql構文構築に失敗しました. SQL:" + sql + " sqlite:" + sqlite3_errmsg(this->db);
		return xreturn::error( msg );
	}
	sqlite3_bind_int (stm, 1, sizehash );
	sqlite3_bind_int64 (stm, 2, accesstime );
	sqlite3_bind_int (stm, 3, rank );
	sqlite3_bind_text(stm, 4, typeUTF8.c_str() ,typeUTF8.size() , SQLITE_STATIC);
	sqlite3_bind_text(stm, 5, titleUTF8.c_str() ,titleUTF8.size() , SQLITE_STATIC);
	sqlite3_bind_text(stm, 6, artistUTF8.c_str() ,artistUTF8.size() , SQLITE_STATIC);
	sqlite3_bind_text(stm, 7, albumUTF8.c_str() ,albumUTF8.size() , SQLITE_STATIC);
	sqlite3_bind_text(stm, 8, aliasUTF8.c_str() ,aliasUTF8.size() , SQLITE_STATIC);
	sqlite3_bind_int(stm, 9, part);
	sqlite3_bind_text(stm, 10, searchdataUTF8.c_str() ,searchdataUTF8.size() , SQLITE_STATIC);
	if (pngImage.empty())
	{
		sqlite3_bind_blob(stm, 11, "", 0 ,SQLITE_STATIC);
	}
	else
	{
		sqlite3_bind_blob(stm, 11, &pngImage[0], pngImage.size() ,SQLITE_STATIC);
	}
	sqlite3_bind_text(stm, 12, dirUTF8.c_str() ,dirUTF8.size() , SQLITE_STATIC);
	sqlite3_bind_text(stm, 13, filenameUTF8.c_str() ,filenameUTF8.size() , SQLITE_STATIC);

	ret = sqlite3_step(stm);
	if (ret != SQLITE_DONE)
	{
		std::string msg =std::string() + "sql発行に失敗しました. SQL:" + sql + " sqlite:" + sqlite3_errmsg(this->db);
		return xreturn::error( msg );
	}
//	unsigned long long id = sqlite3_last_insert_rowid(this->db);
	sqlite3_finalize(stm);
	return true;
}

xreturn::r<bool > MediaFileIndex::SQLUpdateFile(const std::string& dir,const std::string& filename,unsigned long sizehash,time_t accesstime , const std::string& type)
{
	_USE_WINDOWS_ENCODING;

	//ファイルを解析.
	std::string title;
	std::string artist;
	std::string album;
	std::string alias;
	int rank;
	int part;
	std::string searchdata;
	auto r1 = this->Analize.Analize(dir,filename,&title,&artist,&album,&alias,&part,&rank,&searchdata);
	if (!r1)
	{
		this->PoolMainWindow->SyncInvokeLog(std::string() + "ファイル解析中にエラーが発生しました。スキップします。 エラー:" + r1.getErrorMessage() , LOG_LEVEL_ERROR);
	}

	//一覧に出すために、ファイルのスクリーンショットを取得
	std::vector<char> pngImage;
	auto r2 = this->ImageShot.Shot(dir,filename,&pngImage);
	if (!r2)
	{
		this->PoolMainWindow->SyncInvokeLog(std::string() + "サムネイル撮影中にエラーが発生しました。スキップします。 エラー:" + r2.getErrorMessage() , LOG_LEVEL_ERROR);
		pngImage.clear();
	}

	//UTF8に変換
	std::string dirUTF8 = _A2U(dir.c_str());
	std::string filenameUTF8 = _A2U(filename.c_str());
	std::string typeUTF8 = _A2U(type.c_str());
	std::string titleUTF8 = _A2U(title.c_str());
	std::string artistUTF8 = _A2U(artist.c_str());
	std::string albumUTF8 = _A2U(album.c_str());
	std::string aliasUTF8 = _A2U(alias.c_str());
	std::string searchdataUTF8 = _A2U(searchdata.c_str());
	if (!pngImage.empty()){
		rank++;
	}

	//blobがあるからsqlite3_mprintf ではなくて、 bindする
	const char * sql = "UPDATE media SET sizehash=?,accesstime=?,rank=?,type=? ,title=? ,artist=? ,album=? ,alias=? ,part=? ,searchdata=? ,image=? where dir=? AND filename=?";

	sqlite3_stmt* stm = NULL;
	int ret = sqlite3_prepare_v2(this->db,sql, -1, &stm, NULL);
	if (ret != SQLITE_OK)
	{
		std::string msg =std::string() + "sql構文構築に失敗しました. SQL:" + sql + " sqlite:" + sqlite3_errmsg(this->db);
		return xreturn::error( msg );
	}
	sqlite3_bind_int (stm, 1, sizehash );
	sqlite3_bind_int64 (stm, 2, accesstime );
	sqlite3_bind_int (stm, 3, rank );
	sqlite3_bind_text (stm, 4, typeUTF8.c_str() ,typeUTF8.size() , SQLITE_STATIC);
	sqlite3_bind_text(stm, 5, titleUTF8.c_str() ,titleUTF8.size() , SQLITE_STATIC);
	sqlite3_bind_text(stm, 6, artistUTF8.c_str() ,artistUTF8.size() , SQLITE_STATIC);
	sqlite3_bind_text(stm, 7, albumUTF8.c_str() ,albumUTF8.size() , SQLITE_STATIC);
	sqlite3_bind_text(stm, 8, aliasUTF8.c_str() ,aliasUTF8.size() , SQLITE_STATIC);
	sqlite3_bind_int (stm, 9, part );
	sqlite3_bind_text(stm, 10, searchdataUTF8.c_str() ,searchdataUTF8.size() , SQLITE_STATIC);
	if (pngImage.empty())
	{
		sqlite3_bind_blob(stm, 11, "", 0 ,SQLITE_STATIC);
	}
	else
	{
		sqlite3_bind_blob(stm, 11, &pngImage[0], pngImage.size() ,SQLITE_STATIC);
	}

	sqlite3_bind_text(stm, 12, dirUTF8.c_str() ,dirUTF8.size() , SQLITE_STATIC);
	sqlite3_bind_text(stm, 13, filenameUTF8.c_str() ,filenameUTF8.size() , SQLITE_STATIC);

	ret = sqlite3_step(stm);
	if (ret != SQLITE_DONE)
	{
		std::string msg =std::string() + "sql発行に失敗しました. SQL:" + sql + " sqlite:" + sqlite3_errmsg(this->db);
		return xreturn::error( msg );
	}
	sqlite3_finalize(stm);

	return true;
}

xreturn::r<bool > MediaFileIndex::SQLDeleteFile(const std::string& dir,const std::string& filename)
{
	_USE_WINDOWS_ENCODING;

	//UTF8に変換
	std::string dirUTF8 = _A2U(dir.c_str());
	std::string filenameUTF8 = _A2U(filename.c_str());

	//blobがあるからsqlite3_mprintf ではなくて、 bindする
	const char * sql = "DELETE media  where dir=? AND filename=?";

	sqlite3_stmt* stm = NULL;
	int ret = sqlite3_prepare_v2(this->db,sql, -1, &stm, NULL);
	if (ret != SQLITE_OK)
	{
		std::string msg =std::string() + "sql構文構築に失敗しました. SQL:" + sql + " sqlite:" + sqlite3_errmsg(this->db);
		return xreturn::error( msg );
	}
	sqlite3_bind_text(stm, 1, dirUTF8.c_str() ,dirUTF8.size() , SQLITE_STATIC);
	sqlite3_bind_text(stm, 2, filenameUTF8.c_str() ,filenameUTF8.size() , SQLITE_STATIC);

	ret = sqlite3_step(stm);
	if (ret != SQLITE_DONE)
	{
		std::string msg =std::string() + "sql発行に失敗しました. SQL:" + sql + " sqlite:" + sqlite3_errmsg(this->db);
		return xreturn::error( msg );
	}
	sqlite3_finalize(stm);

	return true;
}

//消えてしまったディレクトリや、除外設定されたディレクトリの検索データを消去します。(検索データだけね)
xreturn::r<bool > MediaFileIndex::ScanAndDeleteLostDirectory()
{
	_USE_WINDOWS_ENCODING;

	//消すべきディレクトリを格納するリスト
	std::list<std::string> targetDirectory;

	//DB内のディレクトリについて調査します。
	{
		const char * sql = "SELECT DISTINCT dir from media";	//重いね・・

		sqlite3_stmt* stm = NULL;
		int ret = sqlite3_prepare_v2(this->db,sql, -1, &stm, NULL);
		if (ret != SQLITE_OK)
		{
			std::string msg =std::string() + "sql発行に失敗しました. SQL:" + sql + " sqlite:" + sqlite3_errmsg(this->db);
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
				if(this->StopFlg)
				{
					break;
				}
				const char* dirUTF8 = (const char*) sqlite3_column_text(stm, 0); //dir
				const std::string dir = _U2A(dirUTF8);

				//検索対象ディレクトリであることを確認します。
				if (! IsScanDirectory(dir) )
				{
					targetDirectory.push_back(dir);
					continue;
				}
				//ディレクトリの存在を確認
				if (! (GetFileAttributes( dir.c_str() ) & FILE_ATTRIBUTE_DIRECTORY) )
				{
					targetDirectory.push_back(dir);
					continue;
				}
			}
		}
		sqlite3_finalize(stm);
	}

	//次に、削除対処のディレクトリをDBから削除していきます。
	for(auto it = targetDirectory.begin();it!=targetDirectory.end();++it)
	{
		if(this->StopFlg)
		{
			break;
		}
		//UTF8に変換
		std::string dirUTF8 = _A2U(it->c_str());
	
		//blobがあるからsqlite3_mprintf ではなくて、 bindする
		const char * sql = "DELETE from media  where dir=?";

		sqlite3_stmt* stm = NULL;
		int ret = sqlite3_prepare_v2(this->db,sql, -1, &stm, NULL);
		if (ret != SQLITE_OK)
		{
			std::string msg =std::string() + "sql構文構築に失敗しました. SQL:" + sql + " sqlite:" + sqlite3_errmsg(this->db);
			return xreturn::error( msg );
		}
		sqlite3_bind_text(stm, 1, dirUTF8.c_str() ,dirUTF8.size() , SQLITE_STATIC);

		ret = sqlite3_step(stm);
		if (ret != SQLITE_DONE)
		{
			std::string msg =std::string() + "sql発行に失敗しました. SQL:" + sql + " sqlite:" + sqlite3_errmsg(this->db);
			return xreturn::error( msg );
		}
		sqlite3_finalize(stm);
	}

	return true;
}

//検索対象ディレクトリであることを確認します。
bool MediaFileIndex::IsScanDirectory(const std::string& dir ) const 
{
	
	for(auto it = this->mediaDirectoryListArray.begin();it!=this->mediaDirectoryListArray.end();++it)
	{
		if ( dir.find(*it) == 0 )
		{
			return true;
		}
	}
	//スキャン対象ではありません。
	return false;
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

	_finddata_t data;
	long handle = _findfirst( path.c_str() , &data );
	if (handle == -1)
	{
		return true;
	}
	do
	{
		const std::string filename = dir + "\\" + data.name;

		if (this->StopFlg)
		{
			//スレッド終了フラグ
			return false;
		}

		if (data.attrib & _A_SUBDIR)
		{
			if ( data.name[0] == '.' && (data.name[1] == '\0' || (data.name[1] == '.' && data.name[2] == '\0') ) )
			{
				continue;
			}
			ScanAndUpdate(dir + "\\" + data.name);
			continue;
		}
		std::string type = GetExtType( data.name );
		if ( type.empty() )
		{//対象外の拡張子
			continue;
		}

		auto it = sqlfind_dir.find(data.name);
		if (it == sqlfind_dir.end() )
		{//ねーよ
			auto r = SQLInsertFile(dir,data.name,data.size,data.time_access,type);
			if (!r)
			{
				return xreturn::error(r.getError());
			}
		}
		else if (it->second.sizehash != data.size)
		{//あるけど更新されている!
			auto r = SQLUpdateFile(dir,data.name,data.size,data.time_access,type);
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
	while( ! _findnext(handle,&data) );
	_findclose(handle);

	//削除されたファイルの情報を消す.
	for(auto it = sqlfind_dir.begin() ; it != sqlfind_dir.end() ; ++it)
	{
		if (!it->second.checked)
		{
			SQLDeleteFile(dir, it->first);
		}
	}

	return true;
}


//拡張子から種類を返す
std::string MediaFileIndex::GetExtType(const std::string& filename) const
{
	const std::string ext = XLStringUtil::strtolower(XLStringUtil::baseext_nodot( filename ) );
	const auto it = this->MediaTargetExt.find( ext );
	if (it == this->MediaTargetExt.end())
	{
		return ""; //ない!!
	}
	return it->second;
}

std::string MediaFileIndex::GetDefaultIcon(const std::string& type) const
{
	if ( type == "music")
	{
		return shareIconBase64Music;
	}
	else if ( type == "video")
	{
		return shareIconBase64Video;
	}
	else if ( type == "book")
	{
		return shareIconBase64Book;
	}
	else
	{
		return g_Base64BadIcon; //ない!!
	}
}

//sql escape (prepare bind が最高なのはわかるけど、複雑なSQL組立だと難しいよ・・・ PHPだともう少しうまく書けるフレームワークがあるけども。)
std::string MediaFileIndex::SQLAppend(const char * sql , ...) const
{
	va_list ap;
	char *z;
	if( sqlite3_initialize() ) return "";

	va_start(ap, sql);
	z = sqlite3_vmprintf(sql, ap);
	va_end(ap);

	std::string sqlFragment = z;


	sqlite3_free(z);
	return sqlFragment;
}

//リストに音声認識で使うデータを追加します。
void MediaFileIndex::AppendConvertRecognitionList(std::list<std::string> * list , const std::string& text) const
{
	//漢字やカタカナをひらがなにします。
	std::string str2 = XLStringUtil::mb_convert_kana(this->PoolMainWindow->Mecab.KanjiAndKanakanaToHiragana(text),"cHsa");
	//複数パートに分かれそうな所を \t をいれていきます。
	const char* replaceCharTable[] = {
		 "\t"," "
		,"&","あんど"
		,",","\t"
		,"(","\t"
		,")","\t"
		,"[","\t"
		,"]","\t"
		,"{","\t"
		,"}","\t"
		,"｢","\t"
		,"｣","\t"
		,"_","\t"
		,NULL,NULL //終端
	};
	str2 = XLStringUtil::replace(str2,replaceCharTable);

	//いらないキャラクターを消します。
	const char* removeCharTable[] = {
		 "!","\"","#","$","%","&","'","(",")","=","-","~","^","|","\\","[","]","{","}","+",";","*",":","?",",",".","<",">","/","。","、","､","｡","☆","★"
		,"0","1","2","3","4","5","6","7","8","9","･" //数字もいらない
		,NULL //終端
	};
	str2 = XLStringUtil::remove( str2 , removeCharTable );
	if (str2.empty())
	{
		return ;
	}

	//連続するスペースを一つに
	str2 = XLStringUtil::replace(str2,"    "," ");
	str2 = XLStringUtil::replace(str2,"   "," ");
	str2 = XLStringUtil::replace(str2,"  "," ");

	//日本語と日本語の間にあるスペースは \t にする。
	{
		char * p = &str2[0];
		for(; *p ; )
		{
			if (! XLStringUtil::isMultiByte(p) )
			{
				++p;
				continue;
			}
			p = XLStringUtil::nextChar(p);

			char * space = p;
			if ( *space != ' ' )
			{
				continue;
			}

			p++;
			if (! XLStringUtil::isMultiByte(p) )
			{
				continue;
			}
			p = XLStringUtil::nextChar(p);

			*space = '\t';
		}
	}

	//\tで切れそうな所を切ってく。
	std::list<std::string> splitList = XLStringUtil::split("\t",str2);
	for(auto it = splitList.begin() ; it != splitList.end() ; ++it)
	{
		std::string t = XLStringUtil::chop(*it);
		if (t.empty())
		{
			continue;
		}
		if( !XLStringUtil::isMultiByte(t.c_str()) && t.size() <= 3)
		{
			continue;
		}
		list->push_back(t);
	}
}
