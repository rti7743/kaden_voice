// XLStringUtil.cpp: XLStringUtil クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "common.h"
//#include <string>
//#include <deque>
//#include <vector>
//#include <map>
//#include <cassert>
#ifdef _WINDOWS
#include <tchar.h>
#endif
#include "XLStringUtil.h"


//USE_TEST(XLStringUtil)

/*
　　 　 　 ﾊヽ／::::ヽ.ヘ===ｧ
　　 　 　 {::{/≧=＝=≦V:/
　　 　 　 >:´:::::::::::::::::::::::::｀ヽ、
　　　 γ:::::::::::::::::::::::::::::::::::::::::ヽ
　 _／/::::::::::::::::::::::::::::::::::::::::::::::ﾊ　　　　　　モッピー知ってるよ
.　| ll ! :::::::l::::::/|ハ::::::::∧::::i :::::::i　　　　　 みんななんだかんだ言って
　 ､ヾ|:::::::::|:::/｀ト-:::::/ _,X:j:::/:::l                      PHPが大好きなことを。
　　 ヾ:::::::::|≧ｚ　!Ｖ　ｚ≦　/::::/
　　　 ∧::::ト “　 　 　 　 “ ﾉ:::/!
　 　 /::::（＼　　 ー'　 　/￣）　 |
　　　 　 |　｀`ー――‐''|　　ヽ、.|
　　　　　 ゝ　ノ　　　　 ヽ　 ノ　|
￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣ 　　　　　　　

*/

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

XLStringUtil::XLStringUtil()
{
}

XLStringUtil::~XLStringUtil()
{

}


//みんな大好きPHPのstrtoupper
std::string XLStringUtil::strtoupper(const std::string & str)
{
	if (str.empty())
	{
		return str;
	}
	std::string r = str;
	
	char * p = &r[0];
	for(; *p ; ++p)
	{
		*p = toupper(*p);
	}
	return r;
}

//みんな大好きPHPのstrtolower
std::string XLStringUtil::strtolower(const std::string & str)
{
	if (str.empty())
	{
		return str;
	}
	std::string r = str;
	
	char * p = &r[0];
	for(; *p ; ++p)
	{
		*p = tolower(*p);
	}
	return r;
}

//HTTPヘッダのキャメル
std::string XLStringUtil::HeaderUpperCamel(const std::string & str)
{
	if (str.empty())
	{
		return str;
	}
	std::string r = str;
	
	char * p = &r[0];
	*p = toupper(*p);
	p++;

	for(; *p ; ++p)
	{
		//Content-Length みたいに - のあとはでっかくするよ。
		if (*p == '-')
		{
			++p;
			*p = toupper(*p);
		}
		else
		{
			*p = tolower(*p);
		}
	}
	return r;
}
std::string XLStringUtil::join(const std::string& glue , const std::list<std::string> & pieces )
{
	std::string r;
	r.reserve(pieces.size() * 10);

	std::list<std::string>::const_iterator it = pieces.begin();
	std::list<std::string>::const_iterator et = pieces.end();
	if (it == et)
	{
		return r;
	}
	//最初の一発目.
	r += *it;
	++it;

	for( ; it != et ; ++it )
	{
		r += glue;
		r += *it;
	}
	return r;
}

//key=value& みたいな感じの join
std::string XLStringUtil::crossjoin(const std::string& glue1 ,const std::string& glue2 , const std::map<std::string,std::string> & pieces )
{
	std::string r;
	r.reserve(pieces.size() * 10);

	std::map<std::string,std::string>::const_iterator it = pieces.begin();
	std::map<std::string,std::string>::const_iterator et = pieces.end();
	if (it == et)
	{
		return r;
	}
	//最初の一発目.
	r += it->first + glue1 + it->second;
	++it;

	for( ; it != et ; ++it )
	{
		r += glue2;
		r += it->first + glue1 + it->second;
	}
	return r;
}
//split
std::list<std::string> XLStringUtil::split(const std::string& glue, const std::string & inTarget )
{
	std::list<std::string> r;

	int oldpos = 0;
	int pos = 0;
	while( (pos = inTarget.find( glue , oldpos)) != -1 )
	{
		std::string k = inTarget.substr(oldpos , pos - oldpos);

		r.push_back(k);

		oldpos = pos+glue.size();
	}
	//最後の残り
	{
		std::string k = inTarget.substr(oldpos , pos - oldpos);
		r.push_back(k);
	}
	return r;
}

//key=value& みたいな感じの split
std::map<std::string,std::string> XLStringUtil::crosssplit(const std::string& glue1 ,const std::string& glue2 , const std::string & inTarget )
{
	std::map<std::string,std::string> r;

	int oldpos = 0;
	int pos = 0;
	while( (pos = inTarget.find( glue1 , oldpos)) != -1 )
	{
		std::string k = inTarget.substr(oldpos , pos - oldpos);
		int vpos = k.find( glue2 );
		if (vpos < 0)
		{
			continue;
		}

		std::string v = k.substr( vpos + glue2.size() );
		k = k.substr(0 , vpos);
		r.insert( std::pair<std::string,std::string>(k,v) );

		oldpos = pos + glue1.size();
	}

	//最後の残り
	{
		std::string k = inTarget.substr(oldpos);
		int vpos = k.find( glue2 );
		if (vpos < 0)
		{
			return r;
		}

		std::string v = k.substr( vpos + glue2.size() );
		k = k.substr(0 , vpos);
		r.insert( std::pair<std::string,std::string>(k,v) );
	}
	return r;
}

SEXYTEST("XLStringUtil::crosssplitのてすと")
{
	{
		std::map<std::string,std::string> a = XLStringUtil::crosssplit("&","=","a=1&bb=22&ccc=333");
		std::map<std::string,std::string> b ; b["a"]="1"; b["bb"]="22"; b["ccc"]="333";
		SEXYTEST_EQ(a ,b); 
	}
}


std::map<std::string,std::string> XLStringUtil::merge(const std::map<std::string,std::string>& a ,const std::map<std::string,std::string>& b , bool overideB = true )
{
	if (overideB)
	{
		std::map<std::string,std::string> r = b;
		r.insert(a.begin(),a.end());
		return r;
	}

	std::map<std::string,std::string> r = a;
	r.insert(b.begin(),b.end());
	return r;
}

#define TRIM_SPACE(x)	((x) == ' ' || (x) == '\t' || (x) == '\r' || (x) == '\n')

//みんな大好きPHPのurldecode
std::string XLStringUtil::urldecode(const std::string & inUrl)
{
	static const char xc[] = "0123456789ABCDEF";

	const char* url = inUrl.c_str();
	std::vector<char> buffer;
	buffer.resize(inUrl.size() + 1);

	char* orinal = &buffer[0];
	char* nomal = orinal;


	for( ; *url ; url++ , nomal++)
	{
		if (*url != '%')
		{
			if (*url == '+')	*nomal = ' ';
			else				*nomal = *url;
		}
		else
		{
			//% だけで終わっている文字列の排除
			if ( *(url+1) == 0 ){	*nomal = '%';	continue;	}

			//%Z とかのわけわかめの排除
			const char* first = strchr(xc,*(url+1));
			if ( first == NULL ) {	*nomal = '%';	continue;	}

			//%A だけで終わっている文字列の排除
			if ( *(url+2) == 0 ){	*nomal = '%';	continue;	}

			//%AZ とかのわけわかめの排除
			const char* second = strchr(xc,*(url+2));
			if ( second == NULL ) {	*nomal = '%';	continue;	}

			*nomal = 16 * (first - xc) + (second - xc);
			url ++;
			url ++;
		}
	}
	*nomal = '\0';

	return nomal;
}
//http://d.hatena.ne.jp/ytakano/20081016/urlencode より
std::string XLStringUtil::urlencode(const std::string &str) 
{
    std::ostringstream os;

    for (unsigned int i = 0; i < str.size(); i++) {
        char c = str[i];
        if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
            (c >= '0' && c <= '9') ||
            c == '-' || c == '_' || c == '.' || c == '~') {
            os << c;
        } else {
            char s[4];
            _snprintf(s, sizeof(s), "%%%02x", c & 0xff);
            os << s;
        }
    }

    return os.str();
}

//みんな大好きPHPのchop 左右の空白の除去
std::string XLStringUtil::chop(const std::string & str)
{
	const char * p = str.c_str();
	int size = str.size();
	int l;
	for(l = 0 ; l < size ; l++)
	{
		if ( !TRIM_SPACE(p[l]) )
		{
			break;
		}
	}
	if (l == size)
	{
		return "";	//すべて空白
	}

	int r;
	for(r = size-1 ; r >= 0 ; r--)
	{
		if ( !TRIM_SPACE(p[r]) )
		{
			break;
		}
	}
//	assert (l  <= r);

	return str.substr(l , r - l + 1);
}

//なぜか std::string に標準で用意されていない置換。ふぁっく。
std::string XLStringUtil::replace(const std::string &inTarget ,const std::string &inOld ,const std::string &inNew)
{
	std::string ret;
	ret.reserve( inTarget.size() );	//先読み.

	int full = inTarget.size();
	int oldpos = 0;
	int pos = 0;
	//置換される文字列の長さ
	int replaceTargetSize = inOld.size();

	while( (pos = inTarget.find( inOld , oldpos)) != -1 )
	{
		ret += inTarget.substr(oldpos , pos - oldpos);
		ret += inNew;

		oldpos = pos + replaceTargetSize ;
	}
	ret += inTarget.substr(oldpos , full - oldpos);
	return ret;

}

//みんな大好きPHPのhtmlspecialchars
//タグをエスケープ 基本的に PHP の htmlspecialchars と同じ.
//http://search.net-newbie.com/php/function.htmlspecialchars.html
std::string XLStringUtil::htmlspecialchars(const std::string &inStr)
{
	return replace(replace(replace(replace(inStr , ">" , "&gt;") , "<" , "&lt;") , "\"", "&quot;"), "'","&apos;");
}


//みんな大好きPHPのnl2br
//\nを<br>に 基本的に PHP の nl2br と同じ.
std::string XLStringUtil::nl2br(const std::string &inStr)
{
	return replace(inStr , "\r\n" , "<br>");
}

//inTarget の inStart ～ inEnd までを取得
std::string XLStringUtil::cut(const std::string &inTarget , const std::string & inStart , const std::string &inEnd , std::string * outNext )
{
	const char * p = NULL;
	if ( !inStart.empty() )
	{
		p = strstr(inTarget.c_str() , inStart.c_str() );
		if (p == NULL ) return "";
		p += inStart.size();
	}
	else
	{
		p = inTarget.c_str();
	}

	const char * p2 = NULL;
	if ( ! inEnd.empty() )
	{
		p2 = strstr(p , inEnd.c_str() );
		if (p2 == NULL ) return "";
	}
	else
	{
		p2 = p + strlen(p);
	}

	std::string ret = std::string( p , 0 , p2 - p );
	if (outNext)
	{
		*outNext = std::string(p2 + inEnd.size());
	}

	return ret;
}

//拡張子を取得する. abc.cpp -> ".cpp" のような感じになるよ
std::string XLStringUtil::baseext(const std::string &fullpath)
{
#ifdef _WINDOWS
	//SJISだとこんな感じかな・・・
	const char * p = _tcsrchr(fullpath.c_str() , '.');
#else
	//UTF-8だと仮定してやるよw
	const char * p = strrchr(fullpath.c_str() , '.');
#endif
	if (!p) return "";
	return p;
}

//ベースディレクトリを取得する  c:\\hoge\\hoge.txt -> c:\\hoge にする  最後の\\ は消える。
std::string XLStringUtil::basedir(const std::string &fullpath)
{
#ifdef _WINDOWS
	//SJISだとこんな感じかな・・・
	const char * p = _tcsrchr(fullpath.c_str() , '\\');
#else
	//UTF-8だと仮定してやるよw
	const char * p = strrchr(fullpath.c_str() , '/');
#endif
	if (!p) return "";
	return fullpath.substr(0, (unsigned int) (p - fullpath.c_str()) );
}

//ファイル名を取得する  c:\\hoge\\hoge.txt -> hoge.txt
std::string XLStringUtil::basename(const std::string &fullpath)
{
#ifdef _WINDOWS
	//SJISだとこんな感じかな・・・
	const char * p = _tcsrchr(fullpath.c_str() , '\\');
#else
	//UTF-8だと仮定してやるよw
	const char * p = strrchr(fullpath.c_str() , '/');
#endif
	if (!p) return "";
	return p + 1;
}

std::string XLStringUtil::pathcombine(const std::string& base,const std::string& dir,const std::string& pathsep)
{
#ifdef _WINDOWS
	std::string _pathsep = pathsep.empty() ? "\\" : pathsep;
#else
	std::string _pathsep = pathsep.empty() ? "/" : pathsep;
#endif
	
	std::list<std::string> nodes = split(_pathsep,base + _pathsep + dir);
	std::list<std::string>::iterator i = nodes.begin();
	std::list<std::string> useNodes;
	for( ; i != nodes.end() ; ++ i )
	{
		if ( i->empty() )
		{
			continue;
		}
		if (*i == ".")
		{
			//無視
		}
		else if (*i == "..")
		{
			useNodes.pop_back();
		}
		else
		{
			useNodes.push_back(*i);
		}
	}
	return join(_pathsep,useNodes);
}

//パスの区切り文字を平らにする.
std::string XLStringUtil::pathseparator(const std::string& path)
{
#ifdef _WINDOWS
	return XLStringUtil::replace(path , "/" , "\\");
#else
	return XLStringUtil::replace(path , "\\" , "/");
#endif
}


//URLパラメーターの追加.
std::string XLStringUtil::AppendURLParam(const std::string& url,const std::string& append)
{
	if (url.find("?") >= 0 )
	{
		return url + "&" + append;
	}
	return url + "?" + append;
}

//コマンドライン引数パース
std::list<std::string> XLStringUtil::parse_command(const std::string & command)
{
	enum state
	{
		 NO
		,TARGET
		,QUOTE_SINGLE
		,QUOTE_DOUBLE
	};
	state s = NO;
	const char* targetStart = NULL;
	std::list<std::string> ret;

	const char * p = command.c_str();
	for( ; *p ; ++p)
	{
		switch (s)
		{
		case NO:
			{
				if (*p == '"')
				{
					s = QUOTE_DOUBLE;
				}
				else if (*p == '\'')
				{
					s = QUOTE_SINGLE;
					targetStart = p + 1;
				}
				else if (*p == ' ' || *p == '\t')
				{
					//nop
				}
				else
				{
					s = TARGET;
					targetStart = p;
				}
				break;
			}
		case QUOTE_DOUBLE:
			{
				if (*p == '"')
				{
					assert(targetStart != NULL);
					s = NO;
					ret.push_back( std::string(targetStart ,0 , (int) (p - targetStart - 1) ) );
					targetStart = NULL;
				}
				else if (*p == '\\' && *(p+1) == '"')
				{
					//クウォートスキップ
					p++;
				}
			}
		case QUOTE_SINGLE:
			{
				if (*p == '\'')
				{
					assert(targetStart != NULL);
					s = NO;
					ret.push_back( std::string(targetStart ,0 , (int) (p - targetStart - 1) ) );
					targetStart = NULL;
				}
				else if (*p == '\\' && *(p+1) == '\'')
				{
					//クウォートスキップ
					p++;
				}
			}
		case TARGET:
			{
				if (*p == ' ' || *p == '\t')
				{
					assert(targetStart != NULL);
					s = NO;
					ret.push_back( std::string(targetStart ,0 , (int) (p - targetStart) ) );
					targetStart = NULL;
				}
				else if (*p == '\\' && (*(p+1) == ' ' && *(p+1) == '\t') )
				{
					//クウォートスキップ
					p++;
				}
				break;
			}
		}
	}
	if (s == QUOTE_SINGLE || s == QUOTE_DOUBLE  || s == TARGET)
	{
		assert(targetStart != NULL);
		ret.push_back( std::string(targetStart ,0 , (int) (p - targetStart) ) );
	}
	return ret;
}


/*
void XLStringUtil::test()
{
	{
		std::string r;
		ASSERT( (r = chop("abc") ) == "abc");
		ASSERT( (r = chop(" abc") ) == "abc");
		ASSERT( (r = chop(" abc  ") ) == "abc");
		ASSERT( (r = chop(" abc \r\n") ) == "abc");
		ASSERT( (r = chop("			abc \r\n") ) == "abc");
	}
}
*/