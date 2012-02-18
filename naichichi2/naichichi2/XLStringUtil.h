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
	static std::string XLStringUtil::strtoupper(const std::string & str);

	//みんな大好きPHPのstrtolower
	static std::string XLStringUtil::strtolower(const std::string & str);

	//HTTPヘッダのキャメル
	static std::string XLStringUtil::HeaderUpperCamel(const std::string & str);

	//みんな大好きPHPのjoin
	static std::string XLStringUtil::join(const std::string& glue , const std::map<std::string,std::string> & pieces );
	static std::string XLStringUtil::join(const std::string& glue , const std::list<std::string> & pieces );

	//key=value& みたいな感じの join
	static std::string XLStringUtil::crossjoin(const std::string& glue1 ,const std::string& glue2 , const std::map<std::string,std::string> & pieces );
	//split
	static std::list<std::string> XLStringUtil::split(const std::string& glue, const std::string & inTarget );
	//key=value& みたいな感じの join
	static std::map<std::string,std::string> XLStringUtil::crosssplit(const std::string& glue1 ,const std::string& glue2 , const std::string & inTarget );
	//stringmap 同士のマージ
	static std::map<std::string,std::string> XLStringUtil::merge(const std::map<std::string,std::string>& a ,const std::map<std::string,std::string>& b , bool overideB );

	//みんな大好きPHPのurldecode
	static std::string XLStringUtil::urldecode(const std::string & inUrl);
	static std::string XLStringUtil::urlencode(const std::string &str) ;

	//みんな大好きPHPのchop 左右の空白の除去
	static std::string XLStringUtil::chop(const std::string & str);

	//なぜか std::string に標準で用意されていない置換。ふぁっく。
	static std::string XLStringUtil::replace(const std::string &inTarget ,const std::string &inOld ,const std::string &inNew);

	//みんな大好きPHPのhtmlspecialchars
	//タグをエスケープ 基本的に PHP の htmlspecialchars と同じ.
	//http://search.net-newbie.com/php/function.htmlspecialchars.html
	static std::string XLStringUtil::htmlspecialchars(const std::string &inStr);
	static std::string XLStringUtil::getext(const std::string &inTarget);

	//みんな大好きPHPのnl2br
	//\nを<br>に 基本的に PHP の nl2br と同じ.
	static std::string XLStringUtil::nl2br(const std::string &inStr);
	
	//拡張子を取得する. abc.cpp -> ".cpp" のような感じになるよ
	static std::string XLStringUtil::baseext(const std::string &fullpath);
	//ベースディレクトリを取得する  c:\\hoge\\hoge.txt -> c:\\hoge にする  最後の\\ は消える。
	static std::string XLStringUtil::basedir(const std::string &fullpath);
	//ファイル名を取得する  c:\\hoge\\hoge.txt -> hoge.txt
	static std::string XLStringUtil::basename(const std::string &fullpath);
	//相対パスから絶対パスに変換する
	static std::string XLStringUtil::pathcombine(const std::string& base,const std::string& dir,const std::string& pathsep = "");
	//パスの区切り文字を平らにする.
	static std::string XLStringUtil::pathseparator(const std::string& path);
	//URLパラメーターの追加.
	static std::string XLStringUtil::AppendURLParam(const std::string& url,const std::string& append);


	//inTarget の inStart ～ inEnd までを取得
	static std::string XLStringUtil::cut(const std::string &inTarget , const std::string & inStart , const std::string &inEnd , std::string * outNext );

	//コマンドライン引数パース
	static std::list<std::string> XLStringUtil::parse_command(const std::string & command);

	//	static void test();

};

#endif // !defined(AFX_XLSTRINGUTIL_H__B1B80C81_45F8_4E25_9AD6_FA9AC57294F5__INCLUDED_)
