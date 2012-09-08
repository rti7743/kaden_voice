// XLStringUtil.h: XLStringUtil クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XLSTRINGUTIL_H__B1B80C81_45F8_4E25_9AD6_FA9AC57294F5__INCLUDED_)
#define AFX_XLSTRINGUTIL_H__B1B80C81_45F8_4E25_9AD6_FA9AC57294F5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class XLStringUtil  
{
public:
	XLStringUtil();
	virtual ~XLStringUtil();


	//みんな大好きPHPのstrtoupper
	static std::string strtoupper(const std::string & str);

	//みんな大好きPHPのstrtolower
	static std::string strtolower(const std::string & str);

	static int atoi(const std::string & str);

	//HTTPヘッダのキャメル
	static std::string HeaderUpperCamel(const std::string & str);

	//次の文字列へ
	static inline const char* nextChar(const char * p)
	{
	#ifdef _WINDOWS
		if (((0x81 <= (unsigned char)(*p) && (unsigned char)(*p) <= 0x9f) || (0xe0 <= (unsigned char)(*p) && (unsigned char)(*p) <= 0xfc)))
		{
			return p + 2;
		}
		return p + 1;
	#else
		if ( (((unsigned char)*p) & 0x80) == 0) return p + 1; 
		if ( (((unsigned char)*p) & 0x20) == 0) return p + 2;
		if ( (((unsigned char)*p) & 0x10) == 0) return p + 3;
		if ( (((unsigned char)*p) & 0x08) == 0) return p + 4;
		if ( (((unsigned char)*p) & 0x04) == 0) return p + 5;
		return p + 6;
	#endif
	}
	//次の文字列へ
	static inline char* nextChar(char * p)
	{
		return (char*)nextChar((const char*)p);
	}
	//マルチバイトか？
	static inline bool isMultiByte(const char * p)
	{
	#ifdef _WINDOWS
		return ((0x81 <= (unsigned char)(*p) && (unsigned char)(*p) <= 0x9f) || (0xe0 <= (unsigned char)(*p) && (unsigned char)(*p) <= 0xfc));
	#else
		return ( (((unsigned char)*p) & 0x80) != 0); 
	#endif
	}

	//strstrのマルチバイトセーフ 文字列検索
	static const char* strstr(const std::string& target, const std::string & need );
	//strstrのマルチバイトセーフ 文字列検索
	static const char* strstr(const char* target, const char* need );
	//strstrのマルチバイトセーフ 文字列検索 //若干非効率
	static const char* strrstr(const std::string& target, const std::string & need );
	//strstrのマルチバイトセーフ 文字列検索 //若干非効率
	static const char* strrstr(const char* target, const char* need );
	//stristrのマルチバイトセーフ 大文字小文字関係なしの検索
	static const char* stristr(const std::string& target, const std::string & need );
	//strchrのマルチバイトセーフ 文字列の最初ら検索して最初見に使った文字の位置
	static const char* strchr(const std::string& target, char need );
	//strchrのマルチバイトセーフ 文字列の最初ら検索して最初見に使った文字の位置
	static const char* strchr(const char* target, char need );
	//strrchrのマルチバイトセーフ 文字列の後ろから検索して最初見に使った文字の位置
	static const char* strrchr(const std::string& target, char need );
	//strrchrのマルチバイトセーフ 文字列の後ろから検索して最初見に使った文字の位置
	static const char* strrchr(const char* target, char need );
	//strposのマルチバイトセーフ (strposはPHPにアルやつね)
	//最初に見つかった場所。見つからなければ -1 を返します。
	static int strpos(const std::string& target, const std::string & need );
	//strposのマルチバイトセーフ (strposはPHPにアルやつね)
	//最初に見つかった場所。見つからなければ -1 を返します。
	static int strpos(const char* target, const char* need );
	//strrposのマルチバイトセーフ (strposはPHPにアルやつね)
	//逆から検索して最初に見つかった場所。見つからなければ -1 を返します。
	static int strrpos(const std::string& target, const std::string & need );
	//strrposのマルチバイトセーフ (strposはPHPにアルやつね)
	//逆から検索して最初に見つかった場所。見つからなければ -1 を返します。
	static int strrpos(const char* target, const char* need );
	//striposのマルチバイトセーフ (striposはPHPにアルやつね)
	//大文字小文字関係なしで検索して最初に見つかった場所。見つからなければ -1 を返します。
	static int stripos(const std::string& target, const std::string & need );

	//みんな大好きPHPのjoin
	static std::string join(const std::string& glue , const std::map<std::string,std::string> & pieces );
	static std::string join(const std::string& glue , const std::list<std::string> & pieces );
	//template って知ってるけど、とりあずこれで。
	static std::string join(const std::string& glue , const std::vector<std::string> & pieces );

	//key=value& みたいな感じの join
	static std::string crossjoin(const std::string& glue1 ,const std::string& glue2 , const std::map<std::string,std::string> & pieces );
	//split
	static std::list<std::string> split(const std::string& glue, const std::string & inTarget );
	//vector
	static std::vector<std::string> split_vector(const std::string& glue, const std::string & inTarget );
	//key=value& みたいな感じの split
	static std::map<std::string,std::string> crosssplit(const std::string& glue1 ,const std::string& glue2 , const std::string & inTarget );
	//key=value& みたいな感じの split キーに対するchopを実行する
	static std::map<std::string,std::string> crosssplitChop(const std::string& glue1 ,const std::string& glue2 , const std::string & inTarget );
	//stringmap 同士のマージ
	static std::map<std::string,std::string> merge(const std::map<std::string,std::string>& a ,const std::map<std::string,std::string>& b , bool overideB );

	//みんな大好きPHPのurldecode
	static std::string urldecode(const std::string & inUrl);
	static std::string urlencode(const std::string &str) ;
	//base64エンコード
	static std::string base64encode(const std::string& src) ;
	//base64エンコード
	static std::string base64encode(const char* src,int len) ;
	//base64デコード
	static std::string base64decode(const std::string& src) ;
	//base64デコード
	static void base64decode(const std::string& src ,std::vector<char>* out) ;
	//巡回して、関数 func を適応。 true を返したものだけを結合する。
	static std::string string_filter(const std::list<std::string>& list,const std::function<bool(const std::string&)>& func);
	//巡回して、関数 func を適応。 funcの戻り文字列を結合します。
	static std::string string_map(const std::list<std::string>& list,const std::function<std::string (const std::string&)>& func);
	//巡回して、関数 func を適応。 true を返したものだけを返す。
	static std::list<std::string> array_filter(const std::list<std::string>& list,const std::function<bool(const std::string&)>& func);
	//巡回して、関数 func を適応。 funcの戻りで配列作って返します。
	static std::list<std::string> array_map(const std::list<std::string>& list,const std::function<std::string (const std::string&)>& func);


	//みんな大好きPHPのchop 左右の空白の除去
	static std::string chop(const std::string & str,const char * replaceTable = NULL);

	//なぜか std::string に標準で用意されていない置換。ふぁっく。
	static std::string replace(const std::string &inTarget ,const std::string &inOld ,const std::string &inNew);
	//マルチバイト非対応 の文字列置換
	static std::string replace_low(const std::string &inTarget ,const std::string &inOld ,const std::string &inNew);

	//みんな大好きPHPのhtmlspecialchars
	//タグをエスケープ 基本的に PHP の htmlspecialchars と同じ.
	//http://search.net-newbie.com/php/function.htmlspecialchars.html
	static std::string htmlspecialchars(const std::string &inStr);
	//マルチバイト非対応 タグをエスケープ
	static std::string htmlspecialchars_low(const std::string &inStr);

	//みんな大好きPHPのnl2br
	//\nを<br>に 基本的に PHP の nl2br と同じ.
	static std::string nl2br(const std::string &inStr);
	//マルチバイト非対応 \nを<br>に
	static std::string nl2br_low(const std::string &inStr);
	
	//拡張子を取得する. abc.cpp -> ".cpp" のような感じになるよ
	static std::string baseext(const std::string &fullpath);
	//拡張子を取得する. abc.cpp -> "cpp" のような感じになるよ . をつけない
	static std::string baseext_nodot(const std::string &fullpath);
	//拡張子を取得する. abc.Cpp -> "cpp" のような感じになるよ . をつけないで小文字
	static std::string baseext_nodotsmall(const std::string &fullpath);

	//ベースディレクトリを取得する  c:\\hoge\\hoge.txt -> c:\\hoge にする  最後の\\ は消える。
	static std::string basedir(const std::string &fullpath);
	//ファイル名を取得する  c:\\hoge\\hoge.txt -> hoge.txt
	static std::string basename(const std::string &fullpath);
	//ファイル名だけ(拡張子なし)を取得する  c:\\hoge\\hoge.txt -> hoge
	static std::string basenameonly(const std::string &fullpath);
	//先頭から a と b の同一部分の文字数を返す
	static int strmatchpos(const std::string& a,const std::string& b);
	//フルパスかどうか
	static bool isfullpath(const std::string& dir,const std::string& pathsep = "");
	//相対パスから絶対パスに変換する
	static std::string pathcombine(const std::string& base,const std::string& dir,const std::string& pathsep = "");
	//パスの区切り文字を平らにする.
	static std::string pathseparator(const std::string& path,const std::string& pathsep = "");
	//URLパラメーターの追加.
	static std::string AppendURLParam(const std::string& url,const std::string& append);


	//inTarget の inStart ～ inEnd までを取得
	static std::string cut(const std::string &inTarget , const std::string & inStart , const std::string &inEnd , std::string * outNext );

	//コマンドライン引数パース
	static std::list<std::string> parse_command(const std::string & command);

	//bigramによる文字列分割
	static std::list<std::string> makebigram(const std::string & words);

	//指定した幅で丸める
	static std::string strimwidth(const std::string &  str , int startMoji , int widthMoji ,const std::string& trimmarker);

	//マルチバイト対応 ダブルクウォート
	static std::string doublequote(const std::string& str);
	//非マルチバイトのダブルクウォート
	static std::string doublequote_low(const std::string& str);
	//quoteをはがす
	static std::string dequote(const std::string& str);

	//重複削除
	static std::list<std::string> unique(const std::list<std::string>& list);
	//マルチバイト対応 inOldにマッチしたものがあったら消します
	static std::string remove(const std::string &inTarget ,const std::string &inOld );
	//マルチバイト対応 複数の候補を一括置換 const char * replacetable[] = { "A","あ"  ,"I","い"  , "上","うえ"  , NULL , NULL}  //必ず2つ揃えで書いてね
	static std::string replace(const std::string &inTarget ,const char** replacetable,bool isrev = false);
	//remove 複数の候補を一括削除  const char * replacetable[] = {"A","B","あ","うえお" , NULL}
	static std::string remove(const std::string &inTarget ,const char** replacetable);
	//typo修正
	//r	 「ローマ字」を「ひらがな」に変換します。
	//R	 「ひらがな」を「ローマ字」に変換します。
	//k	 「かな入力typo」を「ひらがな」に変換します。
	//K	 「ひらがな」を「かな入力typo」に変換します。
	static std::string mb_convert_typo(const std::string &inTarget,const std::string& option);
	//みんな大好き PHPのmb_convert_kanaの移植
	//n	 「全角」数字を「半角」に変換します。
	//N	 「半角」数字を「全角」に変換します。
	//a	 「全角」英数字を「半角」に変換します。
	//A	 「半角」英数字を「全角」に変換します 
	//s	 「全角」スペースを「半角」に変換します（U+3000 -> U+0020）。
	//S	 「半角」スペースを「全角」に変換します（U+0020 -> U+3000）。
	//k	 「全角カタカナ」を「半角カタカナ」に変換します。
	//K	 「半角カタカナ」を「全角カタカナ」に変換します。
	//h	 「全角ひらがな」を「半角カタカナ」に変換します。
	//H	 「半角カタカナ」を「全角ひらがな」に変換します。
	//c	 「全角カタカナ」を「全角ひらがな」に変換します。
	//C	 「全角ひらがな」を「全角カタカナ」に変換します。
	static std::string mb_convert_kana(const std::string &inTarget,const std::string& option);
	//みんな大好きPHPのescapeshellarg
	static std::string escapeshellarg(const std::string &inStr);
	//数字の桁数を求める
	static int getScaler(unsigned int num);

	//findfirstとかのワイルドカードを使った文字列比較
	static bool findFilter(const std::string& base,const std::string& filter);

	//時刻を文字列に変換
	static std::string timetostr(time_t time,const std::string & format);

};

#endif // !defined(AFX_XLSTRINGUTIL_H__B1B80C81_45F8_4E25_9AD6_FA9AC57294F5__INCLUDED_)
