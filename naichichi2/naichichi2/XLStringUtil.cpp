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
#include <mbstring.h>
#else
#include <string.h>
#endif
#include "XLStringUtil.h"
#include "XLFileUtil.h"

/*
　　 　 　 ﾊヽ／::::ヽ.ヘ===ｧ
　　 　 　 {::{/≧=＝=≦V:/
　　 　 　 >:´:::::::::::::::::::::::::｀ヽ、
　　　 γ:::::::::::::::::::::::::::::::::::::::::ヽ
　 _／/::::::::::::::::::::::::::::::::::::::::::::::ﾊ　　　　　　モッピー知ってるよ
.　| ll ! :::::::l::::::/|ハ::::::::∧::::i :::::::i　　　　　 なんだかんだ言って
　 ､ヾ|:::::::::|:::/｀ト-:::::/ _,X:j:::/:::l                      みんなPHPが大好きなことを。
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
	for(; *p ; )
	{
		if (isMultiByte(p))
		{
			p = nextChar(p);
		}
		else
		{
			*p = toupper(*p);
			p ++;
		}
	}
	return r;
}
/*
SEXYTEST(XLStringUtil__strtoupper,"XLStringUtil::strtoupperのてすと")
{
	{
		std::string a = XLStringUtil::strtoupper("aBcDefg");
		std::string b = "ABCDEFG";
		SEXYTEST_EQ(a ,b); 
	}
}
*/

//みんな大好きPHPのstrtolower
std::string XLStringUtil::strtolower(const std::string & str)
{
	if (str.empty())
	{
		return str;
	}
	std::string r = str;
	
	char * p = &r[0];
	for(; *p ; )
	{
		if (isMultiByte(p))
		{
			p = nextChar(p);
		}
		else
		{
			*p = tolower(*p);
			p ++;
		}
	}
	return r;
}
/*
SEXYTEST(XLStringUtil__strtolower,"XLStringUtil::strtolowerのてすと")
{
	{
		std::string a = XLStringUtil::strtolower("aBcDefg");
		std::string b = "abcdefg";
		SEXYTEST_EQ(a ,b); 
	}
}
*/

int XLStringUtil::atoi(const std::string & str)
{
	return std::atoi(str.c_str() );
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
/*
SEXYTEST(XLStringUtil__strtoupper,"XLStringUtil::strtoupperのてすと")
{
	{
		std::string a = XLStringUtil::HeaderUpperCamel("content-length");
		std::string b = "Content-Length";
		SEXYTEST_EQ(a ,b); 
	}
	{
		std::string a = XLStringUtil::HeaderUpperCamel("Content-length");
		std::string b = "Content-Length";
		SEXYTEST_EQ(a ,b); 
	}
	{
		std::string a = XLStringUtil::HeaderUpperCamel("content-Length");
		std::string b = "Content-Length";
		SEXYTEST_EQ(a ,b); 
	}
	{
		std::string a = XLStringUtil::HeaderUpperCamel("server");
		std::string b = "Server";
		SEXYTEST_EQ(a ,b); 
	}
}
*/

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

//template って知ってるけど、とりあずこれで。
std::string XLStringUtil::join(const std::string& glue , const std::vector<std::string> & pieces )
{
	std::string r;
	r.reserve(pieces.size() * 10);

	std::vector<std::string>::const_iterator it = pieces.begin();
	std::vector<std::string>::const_iterator et = pieces.end();
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
//strstrのマルチバイトセーフ 文字列検索
const char* XLStringUtil::strstr(const std::string& target, const std::string & need )
{
#ifdef _WINDOWS
	//SJISだとこんな感じかな・・・
	return (const char*) _mbsstr( (unsigned char*) target.c_str() ,(unsigned char*) need.c_str() );
#else
	//UTF-8だと仮定してやるよw
	return std::strstr( target.c_str() ,need.c_str() );
#endif
}

//strstrのマルチバイトセーフ 文字列検索
const char* XLStringUtil::strstr(const char* target, const char* need )
{
#ifdef _WINDOWS
	//SJISだとこんな感じかな・・・
	return (const char*) _mbsstr( (unsigned char*)target ,(unsigned char*) need );
#else
	//UTF-8だと仮定してやるよw
	return std::strstr(target ,need );
#endif
}

//strstrのマルチバイトセーフ 文字列検索 //若干非効率
const char* XLStringUtil::strrstr(const std::string& target, const std::string & need )
{
	if (need.empty()) return NULL;
	char _need0 = need[0];

	const char * start = target.c_str();
	const char * p = start;

	while(start <= p)
	{
		const char * match = strrchr(p,_need0);
		if (!match) return NULL;
		if ( strcmp(match,need.c_str() ) == 0 )	return match;
		p = match - 1;
	}
	return NULL;
}

//strstrのマルチバイトセーフ 文字列検索 //若干非効率
const char* XLStringUtil::strrstr(const char* target, const char* need )
{
	if (!*need) return NULL;
	char _need0 = *need;

	const char * p = target;
	while(target <= p)
	{
		const char * match = strrchr(p,_need0);
		if (!match) return NULL;
		if ( strcmp(match,need ) == 0 )	return match;
		p = match - 1;
	}
	return NULL;
}

//stristrのマルチバイトセーフ 大文字小文字関係なしの検索
const char* XLStringUtil::stristr(const std::string& target, const std::string & need )
{
	std::string _target = XLStringUtil::strtolower(target);
	std::string _need = XLStringUtil::strtolower(need);
	const char * find = XLStringUtil::strstr(_target.c_str(),_need.c_str());
	if (!find)
	{
		return NULL;
	}
	const int offset = (int)(find - _target.c_str());
	return target.c_str() + offset;
}

//strchrのマルチバイトセーフ 文字列の最初ら検索して最初見に使った文字の位置
const char* XLStringUtil::strchr(const std::string& target, char need )
{
#ifdef _WINDOWS
	//SJISだとこんな感じかな・・・
	return (const char*) _mbschr( (unsigned char*)target.c_str() ,(unsigned char) need );
#else
	//UTF-8だと仮定してやるよw
	return std::strchr( target.c_str() ,(int)need );
#endif
}

//strchrのマルチバイトセーフ 文字列の最初ら検索して最初見に使った文字の位置
const char* XLStringUtil::strchr(const char* target, char need )
{
#ifdef _WINDOWS
	//SJISだとこんな感じかな・・・
	return (const char*) _mbschr( (unsigned char*)target ,(unsigned char) need );
#else
	//UTF-8だと仮定してやるよw
	return std::strchr( target ,(int)need );
#endif
}

//strrchrのマルチバイトセーフ 文字列の後ろから検索して最初見に使った文字の位置
const char* XLStringUtil::strrchr(const std::string& target, char need )
{
#ifdef _WINDOWS
	//SJISだとこんな感じかな・・・
	return (const char*) _mbsrchr( (unsigned char*)target.c_str() ,(unsigned char) need );
#else
	//UTF-8だと仮定してやるよw
	return std::strrchr( target.c_str() ,(int)need );
#endif
}

//strrchrのマルチバイトセーフ 文字列の後ろから検索して最初見に使った文字の位置
const char* XLStringUtil::strrchr(const char* target, char need )
{
#ifdef _WINDOWS
	//SJISだとこんな感じかな・・・
	return (const char*) _mbsrchr( (unsigned char*)target ,(unsigned char) need );
#else
	//UTF-8だと仮定してやるよw
	return std::strrchr( target ,(int)need );
#endif
}

//strposのマルチバイトセーフ (strposはPHPにアルやつね)
//最初に見つかった場所。見つからなければ -1 を返します。
int XLStringUtil::strpos(const std::string& target, const std::string & need )
{
	const char * p = XLStringUtil::strstr(target.c_str(),need.c_str());
	if (!p) return -1;

	return p - target.c_str();
}

//strposのマルチバイトセーフ (strposはPHPにアルやつね)
//最初に見つかった場所。見つからなければ -1 を返します。
int XLStringUtil::strpos(const char* target, const char* need )
{
	const char * p = XLStringUtil::strstr(target,need);
	if (!p) return -1;

	return p - target;
}
//strrposのマルチバイトセーフ (strposはPHPにアルやつね)
//逆から検索して最初に見つかった場所。見つからなければ -1 を返します。
int XLStringUtil::strrpos(const std::string& target, const std::string & need )
{
	const char * p = strrstr(target.c_str(),need.c_str());
	if (!p) return -1;

	return p - target.c_str();
}

//strrposのマルチバイトセーフ (strposはPHPにアルやつね)
//逆から検索して最初に見つかった場所。見つからなければ -1 を返します。
int XLStringUtil::strrpos(const char* target, const char* need )
{
	const char * p = strrstr(target,need);
	if (!p) return -1;

	return p - target;
}

//striposのマルチバイトセーフ (striposはPHPにアルやつね)
//大文字小文字関係なしで検索して最初に見つかった場所。見つからなければ -1 を返します。
int XLStringUtil::stripos(const std::string& target, const std::string & need )
{
	const char * p = XLStringUtil::stristr(target,need);
	if (!p) return -1;

	return p - target.c_str();
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
//vector
std::vector<std::string> XLStringUtil::split_vector(const std::string& glue, const std::string & inTarget )
{
	std::vector<std::string> r;

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

		oldpos = pos + glue1.size();

		int vpos = k.find( glue2 );
		if (vpos < 0)
		{
			r.insert( std::pair<std::string,std::string>(k,"") );
			continue;
		}

		std::string v = k.substr( vpos + glue2.size() );
		k = k.substr(0 , vpos);
		r.insert( std::pair<std::string,std::string>(k,v) );

	}

	//最後の残り
	{
		std::string k = inTarget.substr(oldpos);
		int vpos = k.find( glue2 );
		if (vpos < 0)
		{
			r.insert( std::pair<std::string,std::string>(k,"") );
			return r;
		}

		std::string v = k.substr( vpos + glue2.size() );
		k = k.substr(0 , vpos);
		r.insert( std::pair<std::string,std::string>(k,v) );
	}
	return r;
}
/*
SEXYTEST(XLStringUtil__crosssplit,"XLStringUtil::crosssplitのてすと")
{
	
	{
		std::map<std::string,std::string> a = XLStringUtil::crosssplit("&","=","room=&menuエアコン&actionだんぼうMAX");
		std::map<std::string,std::string> b ; b["room"]=""; b["menuエアコン"] = ""; b["actionだんぼうMAX"]="";
		SEXYTEST_EQ(a ,b); 
	}
	{
		std::map<std::string,std::string> a = XLStringUtil::crosssplit("&","=","a=1&bb=22&ccc=333");
		std::map<std::string,std::string> b ; b["a"]="1"; b["bb"]="22"; b["ccc"]="333";
		SEXYTEST_EQ(a ,b); 
	}
}
*/


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


//みんな大好きPHPのurldecode
std::string XLStringUtil::urldecode(const std::string & inUrl)
{
	static const char xc[] = "0123456789ABCDEF";

	const char* url = inUrl.c_str();
	std::vector<char> buffer;
	buffer.resize(inUrl.size() + 1);

	char* start = &buffer[0];
	char* nomal = start;


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

	return start;
}

/*	
SEXYTEST(XLStringUtil__base64encode,"XLStringUtil::base64encodeのてすと")
{
	{
		std::string a = XLStringUtil::urldecode("search=%41%42C%44");
		std::string b = "search=ABCD";
		SEXYTEST_EQ(a ,b); 
	}
}
*/


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

            //_snprintf(s, sizeof(s), "%%%02x", c & 0xff);
#ifdef _WINDOWS
            _snprintf(s, sizeof(s), "%%%02x", c & 0xff);
#else
            snprintf(s, sizeof(s), "%%%02x", c & 0xff);
#endif
			os << s;
        }
    }

    return os.str();
}

//base64エンコード
std::string XLStringUtil::base64encode(const std::string& src) 
{
	return base64encode(src.c_str() , src.size() );
}
/*
SEXYTEST(XLStringUtil__base64encode,"XLStringUtil::base64encodeのてすと")
{
	{
		std::string a = XLStringUtil::base64encode("IE4+, NN4.06+, Gecko, Opera6+");
		std::string b = "SUU0KywgTk40LjA2KywgR2Vja28sIE9wZXJhNis=";
		SEXYTEST_EQ(a ,b); 

		std::string c = XLStringUtil::base64decode(b);
		SEXYTEST_EQ(c ,"IE4+, NN4.06+, Gecko, Opera6+"); 
	}
	{
		std::string a = XLStringUtil::base64encode("ABCDEFG");
		std::string b = "QUJDREVGRw==";
		SEXYTEST_EQ(a ,b); 

		std::string c = XLStringUtil::base64decode(b);
		SEXYTEST_EQ(c ,"ABCDEFG"); 
	}
	{
		std::string a = XLStringUtil::base64encode("http://user1.matsumoto.ne.jp/~goma/js/base64.html");
		std::string b = "aHR0cDovL3VzZXIxLm1hdHN1bW90by5uZS5qcC9+Z29tYS9qcy9iYXNlNjQuaHRtbA==";
		SEXYTEST_EQ(a ,b); 

		std::string c = XLStringUtil::base64decode(b);
		SEXYTEST_EQ(c ,"http://user1.matsumoto.ne.jp/~goma/js/base64.html"); 
	}
	{
		std::vector<char> file;
		XLFileUtil::cat_b("./config/testdata/badimage.jpg",&file);
		SEXYTEST_EQ(file.empty() ,false); 

		//エンコード
		std::string base64 = XLStringUtil::base64encode(&file[0],file.size() );

		//デコード
		std::vector<char> file2;
		XLStringUtil::base64decode(base64 , &file2);

		//比較.
		SEXYTEST_EQ(file.size() ,file2.size()); 
	}
}
*/

//base64エンコード
std::string XLStringUtil::base64encode(const char* src,int len) 
{
	static const char *base64char = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";

	std::vector<char> v;
	v.resize( len * 2 );
	unsigned char * d = (unsigned char *) (&v[0]);

	const unsigned char * p = (unsigned char*) (src);
	const unsigned char * end = (unsigned char*) (p + (len / 3 * 3) );
	for(; p < end ; p += 3 , d += 4)
	{
		*(d + 0)  = base64char[ ((*(p + 0) & 0xfc) >> 2) ];

		*(d + 1)  = base64char[ ((*(p + 0) & 0x03) << 4) |  ((*(p + 1) & 0xf0) >> 4) ];

		*(d + 2)  = base64char[ ((*(p + 1) & 0x0f) << 2) |  ((*(p + 2) & 0xc0) >> 6) ];

		*(d + 3)  = base64char[ ((*(p + 2) & 0x3f)) ];
	}
	//端数を解きます。
	switch ( len % 3 )
	{
	case 2: //1文字足りない
		*(d + 0)  = base64char[ ((*(p + 0) & 0xfc) >> 2) ];

		*(d + 1)  = base64char[ ((*(p + 0) & 0x03) << 4) |  ((*(p + 1) & 0xf0) >> 4) ];

		*(d + 2)  = base64char[ ((*(p + 1) & 0x0f) << 2) ];

		*(d + 3)  = '=';

		d += 4;
		break;
	case 1: //2文字足りない
		*(d + 0)  = base64char[ ((*(p + 0) & 0xfc) >> 2) ];

		*(d + 1)  = base64char[ ((*(p + 0) & 0x03) << 4) ];

		*(d + 2)  = '=';

		*(d + 3)  = '=';

		d += 4;
		break;
	case 0:
		break;
	}

	*d = '\0';

	return &v[0];
}

//base64デコード
std::string XLStringUtil::base64decode(const std::string& src) 
{
	std::vector<char> out;
	base64decode(src,&out);

	return std::string(&out[0] , 0 , out.size() );
}

//base64デコード
void XLStringUtil::base64decode(const std::string& src ,std::vector<char>* out) 
{
	auto base64int = [](unsigned char pp) -> int {
		if (pp >= 'A' && pp <= 'Z') return   pp - 'A';
		else if (pp >= 'a' && pp <= 'z') return   pp - 'a' + 26;
		else if (pp >= '0' && pp <= '9') return   pp - '0' + 52;
		else if (pp == '+') return   62;
		else if (pp == '/') return   63;
		else return 255;
	};

	out->resize( src.size() );
	unsigned char * d = (unsigned char *) (&out->operator[](0) );
	const unsigned char * start = d;
	unsigned int conv0 = 255;
	unsigned int conv1 = 255;
	unsigned int conv2 = 255;
	unsigned int conv3 = 255;

	unsigned char * p = (unsigned char *) src.c_str();
	while(*p)
	{
		conv0 = base64int(*(p + 0));
		if (conv0 == 255) break;

		conv1 = base64int(*(p + 1));
		if (conv1 == 255) break;

		conv2 = base64int(*(p + 2));
		if (conv2 == 255) break;

		conv3 = base64int(*(p + 3));
		if (conv3 == 255) break;

		*(d + 0) = ((conv0) << 2) + ((conv1 & 0x30) >> 4);
		*(d + 1) = ((conv1 & 0x0f) << 4) + ((conv2 & 0x3c) >> 2);
		*(d + 2) = ((conv2 & 0x03) << 6) + ((conv3));

		p += 4;
		d += 3;
	}
	if (conv0 == 255)
	{
	}
	else if (conv1 == 255)
	{
		*(d + 0) = ((conv0) << 2);

		d += 1;
	}
	else if (conv2 == 255)
	{
		*(d + 0) = ((conv0) << 2) + ((conv1 & 0x30) >> 4);
		*(d + 1) = ((conv1 & 0x0f) << 4) ;

		d += 2;
	}
	else if (conv3 == 255)
	{
		*(d + 0) = ((conv0) << 2) + ((conv1 & 0x30) >> 4);
		*(d + 1) = ((conv1 & 0x0f) << 4) + ((conv2 & 0x3c) >> 2);
		*(d + 2) = ((conv2 & 0x03) << 6);

		d += 3;
	}

	//サイズ調整
	out->resize((int) (d - start));
}

//みんな大好きPHPのchop 左右の空白の除去 strのみマルチバイト対応  replaceTableは半角のみ
std::string XLStringUtil::chop(const std::string & str,const char * replaceTable)
{
	if (replaceTable == NULL)
	{
		replaceTable = " \t\r\n";
	}

	const char * p = str.c_str();
	//頭出し
	while(*p)
	{
		if (isMultiByte(p))
		{
			break;
		}
		const char * rep = replaceTable;
		for(; *rep ; ++rep)
		{
			if ( *rep == *p )
			{
				break;
			}
		}
		if (*rep == NULL)
		{//not match
			break;
		}
		p ++;
	}

	const char * start = p;
	const char * lastEffectvie = p;

	//終端削り
	while(*p)
	{
		if (isMultiByte(p))
		{
			p = nextChar(p);
			lastEffectvie = p;
			continue;
		}
		const char * rep = replaceTable;
		for(; *rep ; ++rep)
		{
			if ( *rep == *p )
			{
				break;
			}
		}
		if (*rep == NULL)
		{//not match
			++p;
			lastEffectvie = p;
		}
		else
		{//match
			++p;
		}
	}
	return std::string(start , 0 , (lastEffectvie - start) );
}
/*
SEXYTEST(XLStringUtil__chop,"XLStringUtil::chopのてすと")
{
	{
		std::string a = XLStringUtil::chop("コンピュータ");
		std::string b = "コンピュータ";
		SEXYTEST_EQ(a ,b); 
	}
	{
		std::string a = XLStringUtil::chop("コンピュータ ");
		std::string b = "コンピュータ";
		SEXYTEST_EQ(a ,b); 
	}
	{
		std::string a = XLStringUtil::chop(" コンピュータ");
		std::string b = "コンピュータ";
		SEXYTEST_EQ(a ,b); 
	}
	{
		std::string a = XLStringUtil::chop(" コンピュータ ");
		std::string b = "コンピュータ";
		SEXYTEST_EQ(a ,b); 
	}
	{
		std::string a = XLStringUtil::chop(" コンピュータ A ");
		std::string b = "コンピュータ A";
		SEXYTEST_EQ(a ,b); 
	}
	{
		std::string a = XLStringUtil::chop("abc");
		std::string b = "abc";
		SEXYTEST_EQ(a ,b); 
	}
	{
		std::string a = XLStringUtil::chop(" abc");
		std::string b = "abc";
		SEXYTEST_EQ(a ,b); 
	}
	{
		std::string a = XLStringUtil::chop(" abc  ");
		std::string b = "abc";
		SEXYTEST_EQ(a ,b); 
	}
	{
		std::string a = XLStringUtil::chop(" abc \r\n");
		std::string b = "abc";
		SEXYTEST_EQ(a ,b); 
	}
	{
		std::string a = XLStringUtil::chop("			abc \r\n");
		std::string b = "abc";
		SEXYTEST_EQ(a ,b); 
	}
}
*/

//巡回して、関数 func を適応。 true を返したものだけを結合する。
std::string XLStringUtil::string_filter(const std::list<std::string>& list,const std::function<bool(const std::string&)>& func)
{
	std::string ret;
	for(auto it = list.begin() ; it != list.end() ; ++it )
	{
		if ( func(*it) )
		{
			ret += *it;
		}
	}
	return ret;
}

//巡回して、関数 func を適応。 funcの戻り文字列を結合します。
std::string XLStringUtil::string_map(const std::list<std::string>& list,const std::function<std::string (const std::string&)>& func)
{
	std::string ret;
	for(auto it = list.begin() ; it != list.end() ; ++it )
	{
		ret += func(*it);
	}
	return ret;
}

//巡回して、関数 func を適応。 true を返したものだけを返す。
std::list<std::string> XLStringUtil::array_filter(const std::list<std::string>& list,const std::function<bool(const std::string&)>& func)
{
	std::list<std::string> ret;
	for(auto it = list.begin() ; it != list.end() ; ++it )
	{
		if ( func(*it) )
		{
			ret.push_back(*it);
		}
	}
	return ret;
}

//巡回して、関数 func を適応。 funcの戻りで配列作って返します。
std::list<std::string> XLStringUtil::array_map(const std::list<std::string>& list,const std::function<std::string (const std::string&)>& func)
{
	std::list<std::string> ret;
	for(auto it = list.begin() ; it != list.end() ; ++it )
	{
		ret.push_back(func(*it));
	}
	return ret;
}
//マルチバイト対応 なぜか std::string に標準で用意されていない置換。ふぁっく。
std::string XLStringUtil::replace(const std::string &inTarget ,const std::string &inOld ,const std::string &inNew)
{
	std::string ret;
	ret.reserve( inTarget.size() );	//先読み.

	const char * p = inTarget.c_str();
	const char * match;
	while( match = XLStringUtil::strstr( p , inOld.c_str() ) )
	{
		//ret += std::string(p,0,(int)(match - p));
		ret.append(p,(int)(match - p));
		ret += inNew;

		p = match + inOld.size();
	}
	//残りを足しておしまい.
	return ret + p;
}


//マルチバイト非対応 の文字列置換
std::string XLStringUtil::replace_low(const std::string &inTarget ,const std::string &inOld ,const std::string &inNew)
{
	std::string ret;
	ret.reserve( inTarget.size() );	//先読み.

	const char * p = inTarget.c_str();
	const char * match;
	while( match = std::strstr( p , inOld.c_str() ) )
	{
//		ret += std::string(p,0,(int)(match - p));
		ret.append(p,(int)(match - p));
		ret += inNew;

		p = match + inOld.size();
	}
	//残りを足しておしまい.
	return ret + p;
}

/*
SEXYTEST(XLStringUtil__replace,"XLStringUtil::replace")
{
	{
		std::string a = XLStringUtil::replace("にょろーん","ろー" , "ぱーー");
		SEXYTEST_EQ(a ,"にょぱーーん");
	}
	{
		std::string a = XLStringUtil::replace("ABCDEFG","BCD" , "XYZ");
		SEXYTEST_EQ(a ,"AXYZEFG");
	}
}
*/


//みんな大好きPHPのhtmlspecialchars
//タグをエスケープ 基本的に PHP の htmlspecialchars と同じ.
//http://search.net-newbie.com/php/function.htmlspecialchars.html
std::string XLStringUtil::htmlspecialchars(const std::string &inStr)
{
	return replace(replace(replace(replace(inStr , ">" , "&gt;") , "<" , "&lt;") , "\"", "&quot;"), "'","&apos;");
}

//マルチバイト非対応 タグをエスケープ
std::string XLStringUtil::htmlspecialchars_low(const std::string &inStr)
{
	return replace_low(replace_low(replace_low(replace_low(inStr , ">" , "&gt;") , "<" , "&lt;") , "\"", "&quot;"), "'","&apos;");
}

//みんな大好きPHPのnl2br
//\nを<br>に 基本的に PHP の nl2br と同じ.
std::string XLStringUtil::nl2br(const std::string &inStr)
{
	return replace(inStr , "\r\n" , "<br>");
}

//マルチバイト非対応 \nを<br>に 基本的に PHP の nl2br と同じ.
std::string XLStringUtil::nl2br_low(const std::string &inStr)
{
	return replace_low(inStr , "\r\n" , "<br>");
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
	const char * p = XLStringUtil::strrchr(fullpath.c_str() , '.');
	if (!p) return "";
	return p;
}
/*
SEXYTEST(XLStringUtil__baseext,"XLStringUtil::baseextのてすと")
{
	{
		std::string a = XLStringUtil::baseext("c:\\aaa\\bbb\\ccc.exe");
		std::string b = ".exe";
		SEXYTEST_EQ(a ,b); 
	}
}
*/
//拡張子を取得する. abc.cpp -> "cpp" のような感じになるよ . をつけない
std::string XLStringUtil::baseext_nodot(const std::string &fullpath)
{
	const char * p = XLStringUtil::strrchr(fullpath.c_str() , '.');
	if (!p) return "";
	return p + 1;
}
/*
SEXYTEST(XLStringUtil__baseext_nodot,"XLStringUtil::baseext_nodotのてすと")
{
	{
		std::string a = XLStringUtil::baseext_nodot("c:\\aaa\\bbb\\ccc.exe");
		std::string b = "exe";
		SEXYTEST_EQ(a ,b); 
	}
}
*/

//拡張子を取得する. abc.Cpp -> "cpp" のような感じになるよ . をつけないで小文字
std::string XLStringUtil::baseext_nodotsmall(const std::string &fullpath)
{
	const char * p = XLStringUtil::strrchr(fullpath.c_str() , '.');
	if (!p) return "";
	return XLStringUtil::strtolower(p + 1);
}

//ベースディレクトリを取得する  c:\\hoge\\hoge.txt -> c:\\hoge にする  最後の\\ は消える。
std::string XLStringUtil::basedir(const std::string &fullpath)
{
#ifdef _WINDOWS
	//SJISだとこんな感じかな・・・
	const char * p = XLStringUtil::strrchr(fullpath.c_str() , '\\');
#else
	//UTF-8だと仮定してやるよw
	const char * p = XLStringUtil::strrchr(fullpath.c_str() , '/');
#endif
	if (!p) return "";
	return fullpath.substr(0, (unsigned int) (p - fullpath.c_str()) );
}
/*
SEXYTEST(XLStringUtil__basedir,"XLStringUtil::basedirのてすと")
{
	{
		std::string a = XLStringUtil::basedir("c:\\aaa\\bbb\\ccc.exe");
		std::string b = "c:\\aaa\\bbb";
		SEXYTEST_EQ(a ,b); 
	}
}
*/

//ファイル名を取得する  c:\\hoge\\hoge.txt -> hoge.txt
std::string XLStringUtil::basename(const std::string &fullpath)
{
#ifdef _WINDOWS
	//SJISだとこんな感じかな・・・
	const char * p = XLStringUtil::strrchr(fullpath, '\\');
#else
	//UTF-8だと仮定してやるよw
	const char * p = XLStringUtil::strrchr(fullpath, '/');
#endif
	if (!p) return fullpath;
	return p + 1;
}
/*
SEXYTEST(XLStringUtil__basename,"XLStringUtil::basenameのてすと")
{
	{
		std::string a = XLStringUtil::basename("c:\\aaa\\bbb\\ccc.exe");
		std::string b = "ccc.exe";
		SEXYTEST_EQ(a ,b); 
	}
}
*/

//ファイル名だけ(拡張子なし)を取得する  c:\\hoge\\hoge.txt -> hoge
std::string XLStringUtil::basenameonly(const std::string &fullpath)
{
#ifdef _WINDOWS
	//SJISだとこんな感じかな・・・
	const char * p = XLStringUtil::strrchr(fullpath, '\\');
#else
	//UTF-8だと仮定してやるよw
	const char * p = XLStringUtil::strrchr(fullpath, '/');
#endif
	if (!p) p = fullpath.c_str();
	else p = p + 1;

	const char* ext = XLStringUtil::strrchr(p , '.');
	if (ext == NULL) return p;

	return std::string(p , 0 , ext - p );
}
/*
SEXYTEST(XLStringUtil__basenameonly,"XLStringUtil::basenameonlyのてすと")
{
	{
		std::string a = XLStringUtil::basenameonly("c:\\aaa\\bbb\\ccc.exe");
		std::string b = "ccc";
		SEXYTEST_EQ(a ,b); 
	}
}
*/

//フルパスかどうか
bool XLStringUtil::isfullpath(const std::string& dir,const std::string& pathsep)
{
#ifdef _WINDOWS
	std::string _pathsep = pathsep.empty() ? "\\" : pathsep;
#else
	std::string _pathsep = pathsep.empty() ? "/" : pathsep;
#endif
	std::string _dir = pathseparator(dir,pathsep);

#ifdef _WINDOWS
	if ( XLStringUtil::strpos(_dir,pathsep + pathsep ) == 0 )
	{// \\\\hogehoge
		return true;
	}
	if ( XLStringUtil::strpos(_dir,":" + pathsep ) == 1 )
	{// c:\\ 
		return true;
	}
#else
	if ( XLStringUtil::strpos(_dir,pathsep) == 0 )
	{// /var/hog ...
		return true;
	}
#endif
	return false;
}

//先頭から a と b の同一部分の文字数を返す
int XLStringUtil::strmatchpos(const std::string& a,const std::string& b)
{
	const char * _x;
	const char * _a = a.c_str();
	const char * _b = b.c_str();
	_x = _a;
	while(*_a++ == *_b++ && *_a != 0 );

	return (int)(_x - _a);
}

std::string XLStringUtil::pathcombine(const std::string& base,const std::string& dir,const std::string& pathsep)
{
#ifdef _WINDOWS
	std::string _pathsep = pathsep.empty() ? "\\" : pathsep;
#else
	std::string _pathsep = pathsep.empty() ? "/" : pathsep;
#endif
	std::string _base = pathseparator(base,pathsep);
	std::string _dir = pathseparator(dir,pathsep);

	std::list<std::string> nodes ; 
	if ( XLStringUtil::isfullpath(_dir,_pathsep) )
	{
		nodes = split(_pathsep,_dir);
	}
	else
	{
		nodes = split(_pathsep,_base + _pathsep + _dir);
	}

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
			if ( i == nodes.begin() )
			{//先頭にある ./ だけは残す
				useNodes.push_back(*i);
			}
			else
			{//それ以外の . は無視
			}
		}
		else if (*i == "..")
		{
			if (useNodes.size() == 1)
			{
#ifdef _WINDOWS
				auto topnode = useNodes.begin();
				if ( topnode->size() >= 2 && (topnode->c_str())[1] == ':' )
				{//windowsのc: より上には上がれないので無視
					continue;
				}
#else
				
#endif
				useNodes.pop_back();
				useNodes.push_back("");	//  /を追加.
			}
			else
			{
				useNodes.pop_back();
			}
		}
		else
		{
			useNodes.push_back(*i);
		}
	}
	std::string retpath = join(_pathsep,useNodes);
#ifdef _WINDOWS
#else
	if ( XLStringUtil::isfullpath(_base,_pathsep) )
	{
		retpath = "/" + retpath;
	}
	else
	{
	}
#endif
	return retpath;
}
/*
SEXYTEST(XLStringUtil__pathcombine,"XLStringUtil::pathcombine")
{
	{
		std::string a = XLStringUtil::pathcombine("c:\\Program Files\\hogehoge" , "c:\\Program Files\\hogehoge\\aaa.txt","\\");
		SEXYTEST_EQ(a ,"c:\\Program Files\\hogehoge\\aaa.txt");
	}
	{
		std::string a = XLStringUtil::pathcombine("c:\\Program Files\\hogehoge" , "c:\\Program Files\\hogehoge2\\aaa.txt","\\");
		SEXYTEST_EQ(a ,"c:\\Program Files\\hogehoge2\\aaa.txt");
	}
	{
		std::string a = XLStringUtil::pathcombine("c:\\Program Files\\hogehoge" , "c:\\home\\hogehoge2\\aaa.txt","\\");
		SEXYTEST_EQ(a ,"c:\\home\\hogehoge2\\aaa.txt");
	}
	{
		std::string a = XLStringUtil::pathcombine("./config/webroot" , "./hello.tpl","\\");
		SEXYTEST_EQ(a ,".\\config\\webroot\\hello.tpl");
	}
	{
		std::string a = XLStringUtil::pathcombine("config/webroot" , "./hello.tpl","\\");
		SEXYTEST_EQ(a ,"config\\webroot\\hello.tpl");
	}
	{
		std::string a = XLStringUtil::pathcombine("c:\\Program Files\\hogehoge" , "./hello.tpl","\\");
		SEXYTEST_EQ(a ,"c:\\Program Files\\hogehoge\\hello.tpl");
	}
	{
		std::string a = XLStringUtil::pathcombine("c:\\Program Files\\hogehoge" , "../hello.tpl","\\");
		SEXYTEST_EQ(a ,"c:\\Program Files\\hello.tpl");
	}
	{
		std::string a = XLStringUtil::pathcombine("c:\\Program Files\\hogehoge" , "../../hello.tpl","\\");
		SEXYTEST_EQ(a ,"c:\\hello.tpl");
	}
	{
		std::string a = XLStringUtil::pathcombine("c:\\Program Files\\hogehoge" , "../../../hello.tpl","\\");
		SEXYTEST_EQ(a ,"c:\\hello.tpl");
	}
	{
		std::string a = XLStringUtil::pathcombine("/var/www/html" , "../../../hello.tpl","\\");
		SEXYTEST_EQ(a ,"\\hello.tpl");
	}
}
*/

//パスの区切り文字を平らにする.
std::string XLStringUtil::pathseparator(const std::string& path,const std::string& pathsep)
{
	if ( pathsep.empty() )
	{
#ifdef _WINDOWS
		return XLStringUtil::replace(path , "/" , "\\");
#else
		return XLStringUtil::replace(path , "\\" , "/");
#endif
	}
	if ( pathsep == "/" )
	{
		return XLStringUtil::replace(path , "\\" , "/");
	}
	else if ( pathsep == "\\" )
	{
		return XLStringUtil::replace(path , "/" , "\\");
	}
	else
	{
		assert(0);
		return path;
	}
}


//URLパラメーターの追加.
std::string XLStringUtil::AppendURLParam(const std::string& url,const std::string& append)
{
	if (url.find("?") != -1 )
	{
		return url + "&" + append;
	}
	return url + "?" + append;
}
/*
SEXYTEST(XLStringUtil__AppendURLParam,"XLStringUtil::AppendURLParamのてすと")
{
	{
		std::string a = XLStringUtil::AppendURLParam("http://127.0.0.1:15550/media_start","accesstoken=kaede");
		std::string b = "http://127.0.0.1:15550/media_start?accesstoken=kaede";
		SEXYTEST_EQ(a ,b); 
	}
}
*/

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

//bigramによる文字列分割
std::list<std::string> XLStringUtil::makebigram(const std::string & words)
{
	std::list<std::string> ret;
	const char * s = NULL;
	const char * n = NULL;
	for(const char * p = words.c_str() ; *p ; )
	{
		s = nextChar(p);
		if (*s == 0x00)
		{
			break;
		}
		n = nextChar(s);

		ret.push_back( std::string(p,0,(int)(n-p) ) );

		p = s;
	}
	return ret;
}
/*
SEXYTEST(XLStringUtil__makebigram,"XLStringUtil::makebigramのてすと")
{
	{
		std::list<std::string> a = XLStringUtil::makebigram("abc");
		std::list<std::string> b ; b.push_back("ab"); b.push_back("bc");
		SEXYTEST_EQ(a ,b); 
	}
	{
		std::list<std::string> a = XLStringUtil::makebigram("愛飢え男");
		std::list<std::string> b ; b.push_back("愛飢"); b.push_back("飢え"); b.push_back("え男");
		SEXYTEST_EQ(a ,b); 
	}
	{
		std::list<std::string> a = XLStringUtil::makebigram("ai飢え男");
		std::list<std::string> b ; b.push_back("ai"); b.push_back("i飢"); b.push_back("飢え"); b.push_back("え男");
		SEXYTEST_EQ(a ,b); 
	}
}
*/

//指定した幅で丸める
std::string XLStringUtil::strimwidth(const std::string &  str , int startMoji , int widthMoji ,const std::string& trimmarker)
{
	int countMoji = 0;
	std::string ret;

	//先頭も省略する場合
	if (startMoji >= 1)
	{
		ret = trimmarker;
	}

	//開始位置を見つける
	const char * p;
	for(p = str.c_str() ; *p ;)
	{
		if (countMoji >= startMoji)
		{
			break;
		}
		p = nextChar(p);
		countMoji++;
	}
	const char * start = p;

	//省略位置を見つける
	for( ; *p ; )
	{
		if (countMoji >= startMoji + widthMoji)
		{
			break;
		}
		p = nextChar(p);
		countMoji++;
	}

	//メインとなる文字列を取得
	ret += std::string(start , 0 ,(int) (p - start));

	//後ろを省略している場合
	if (countMoji >= startMoji + widthMoji)
	{
		ret += trimmarker;
	}
	return ret;
}

/*
SEXYTEST(XLStringUtil__strimwidth,"XLStringUtil::strimwidthのてすと")
{
	{
		std::string a = XLStringUtil::strimwidth("あいうえおあお",2,3,"!!");
		std::string b = "!!うえお!!";
		SEXYTEST_EQ(a ,b); 
	}
}
*/

//マルチバイト対応 ダブルクウォート
std::string XLStringUtil::doublequote(const std::string& str)
{
	return replace(str , "\"" , "\\\"" );
}

//非マルチバイトのダブルクウォート
std::string XLStringUtil::doublequote_low(const std::string& str)
{
	return replace_low(str , "\"" , "\\\"" );
}


//重複削除
std::list<std::string> XLStringUtil::unique(const std::list<std::string>& list)
{
	//単純なアルゴリズムなので遅いです。 気にしない!
	std::list<std::string> ret;
	for(std::list<std::string>::const_iterator it = list.begin() ; it != list.end() ; ++it) 
	{
		if ( ret.empty() )
		{
			ret.push_back(*it);
			continue;
		}

		std::list<std::string>::const_iterator itf = ret.begin();
		for( ; itf != ret.end() ; ++itf) 
		{
			if (*it == *itf)
			{
				break;
			}
		}
		if (itf == ret.end())
		{
			ret.push_back(*it);
		}
	}
	return ret;
}
//マルチバイト対応 inOldにマッチしたものがあったら消します
std::string XLStringUtil::remove(const std::string &inTarget ,const std::string &inOld )
{
	std::string ret;
	ret.reserve( inTarget.size() );	//先読み.

	const char * p = inTarget.c_str();
	const char * match;
	while( match = XLStringUtil::strstr( p , inOld.c_str() ) )
	{
		ret += std::string(p,0,(int)(match - p));
		p = match + inOld.size();
	}
	//残りを足しておしまい.
	return ret + p;
}

//マルチバイト対応 複数の候補を一括置換 const char * replacetable[] = { "A","あ"  ,"I","い"  , "上","うえ" , NULL , NULL}  //必ず2つ揃えで書いてね
std::string XLStringUtil::replace(const std::string &inTarget ,const char** replacetable,bool isrev)
{
	std::string ret;
	ret.reserve( inTarget.size() );	//先読み.

	if (inTarget.empty())
	{
		return inTarget;
	}

	const char * p = inTarget.c_str();
	for(; *p ; )
	{
		const char * pp = p;
		p = nextChar(p);

		int compareIndex = isrev == false ? 0 : 1;
		int replaceIndex = isrev == false ? 1 : 0;
		const char ** r1 = replacetable;
		for( ; *r1 != NULL ; r1+=2)
		{
			const char * ppp = pp;
			const char * r2 = *(r1+compareIndex);
			for( ; 1 ; ++r2,++ppp )
			{
				if ( *r2 == NULL || *ppp != *r2)
				{
					break;
				}
			}
			if (*r2 == NULL)  //無事比較文字列の方が終端にたどりついた
			{
				ret.append(*(r1+replaceIndex));
				p = ppp;
				break;
			}
		}
		if ( *r1 == NULL )
		{
			ret.append(pp,(int) (p - pp));
		}
	}
	return ret;
}

/*
SEXYTEST(XLStringUtil__replace,"XLStringUtil::replaceのてすと")
{
	{
		const char *replaceTable[] = {
			 "アイ","あい"
			,"イ","い"
			,NULL,NULL
		};
		std::string a = XLStringUtil::replace("うアイう",replaceTable);
		std::string b = "うあいう";
		SEXYTEST_EQ(a ,b); 
	}
}
*/

//remove 複数の候補を一括削除  const char * replacetable[] = {"A","B","あ","うえお" , NULL} 全部消します
std::string XLStringUtil::remove(const std::string &inTarget ,const char** replacetable)
{
	std::string ret;
	ret.reserve( inTarget.size() );	//先読み.

	if (inTarget.empty())
	{
		return inTarget;
	}

	const char * p = inTarget.c_str();
	for(; *p ; )
	{
		const char * pp = p;
		p = nextChar(p);

		const char ** r1 = replacetable;
		for( ; *r1 != NULL ; r1++)
		{
			const char * ppp = pp;
			const char * r2 = *(r1);
			for( ; 1 ; ++r2,++ppp )
			{
				if ( *r2 == NULL || *ppp != *r2)
				{
					break;
				}
			}
			if (*r2 == NULL)  //無事比較文字列の方が終端にたどりついた
			{
				p = ppp;
				break;
			}
		}
		if ( *r1 == NULL )
		{
			ret.append(pp,(int) (p - pp));
		}
	}
	return ret;
}


//typo修正
//r	 「ローマ字」を「ひらがな」に変換します。
//R	 「ひらがな」を「ローマ字」に変換します。
//k	 「かな入力typo」を「ひらがな」に変換します。
//K	 「ひらがな」を「かな入力typo」に変換します。
std::string XLStringUtil::mb_convert_typo(const std::string &inTarget,const std::string& option)
{
	static const char *replaceTableRoma[] = {
		 "ltsu","っ"
		,"whu","う"
		,"lyi","ぃ"
		,"xyi","ぃ"
		,"lye","ぇ"
		,"xye","ぇ"
		,"wha","うぁ"
		,"whi","うぃ"
		,"whe","うぇ"
		,"who","うぉ"
		,"kyi","きぃ"
		,"kye","きぇ"
		,"kya","きゃ"
		,"kyu","きゅ"
		,"kyo","きょ"
		,"kwa","くぁ"
		,"qwa","くぁ"
		,"qwi","くぃ"
		,"qyi","くぃ"
		,"qwu","くぅ"
		,"qwe","くぇ"
		,"qye","くぇ"
		,"qwo","くぉ"
		,"qya","くゃ"
		,"qyu","くゅ"
		,"qyo","くょ"
		,"syi","しぃ"
		,"swi","しぇ"
		,"sha","しゃ"
		,"shu","しゅ"
		,"sho","しょ"
		,"syi","しぇ"
		,"sya","しゃ"
		,"syu","しゅ"
		,"syo","しょ"
		,"shi","し"
		,"swa","すぁ"
		,"swi","すぃ"
		,"swu","すぅ"
		,"swe","すぇ"
		,"swo","すぉ"
		,"cyi","ちぃ"
		,"tyi","ちぃ"
		,"che","ちぇ"
		,"cye","ちぇ"
		,"tye","ちぇ"
		,"cha","ちゃ"
		,"cya","ちゃ"
		,"tya","ちゃ"
		,"chu","ちゅ"
		,"cyu","ちゅ"
		,"tyu","ちゅ"
		,"cho","ちょ"
		,"cyo","ちょ"
		,"tyo","ちょ"
		,"chi","ち"
		,"tsa","つぁ"
		,"tsi","つぃ"
		,"tse","つぇ"
		,"tso","つぉ"
		,"tsu","つ"
		,"ltu","っ"
		,"xtu","っ"
		,"thi","てぃ"
		,"the","てぇ"
		,"tha","てゃ"
		,"thu","てゅ"
		,"tho","てょ"
		,"twa","とぁ"
		,"twi","とぃ"
		,"twu","とぅ"
		,"twe","とぇ"
		,"two","とぉ"
		,"nyi","にぃ"
		,"nye","にぇ"
		,"nya","にゃ"
		,"nyu","にゅ"
		,"nyo","にょ"
		,"hyi","ひぃ"
		,"hye","ひぇ"
		,"hya","ひゃ"
		,"hyu","ひゅ"
		,"hyo","ひょ"
		,"fwa","ふぁ"
		,"fwi","ふぃ"
		,"fyi","ふぃ"
		,"fwu","ふぅ"
		,"few","ふぇ"
		,"fye","ふぇ"
		,"fwo","ふぉ"
		,"fya","ふゃ"
		,"fyu","ふゅ"
		,"fyo","ふょ"
		,"myi","みぃ"
		,"mye","みぇ"
		,"mya","みゃ"
		,"myu","みゅ"
		,"myo","みょ"
		,"lya","ゃ"
		,"xya","ゃ"
		,"lyu","ゅ"
		,"xyu","ゅ"
		,"lyo","ょ"
		,"xyo","ょ"
		,"ryi","りぃ"
		,"rye","りぇ"
		,"rya","りゃ"
		,"ryu","りゅ"
		,"ryo","りょ"
		,"gyi","ぎぃ"
		,"gye","ぎぇ"
		,"gya","ぎゃ"
		,"gyu","ぎゅ"
		,"gyo","ぎょ"
		,"gwa","ぐぁ"
		,"gwi","ぐぃ"
		,"gwu","ぐぅ"
		,"gwe","ぐぇ"
		,"gwo","ぐぉ"
		,"jyi","じぃ"
		,"zyi","じぃ"
		,"jye","じぇ"
		,"zye","じぇ"
		,"jya","じゃ"
		,"zya","じゃ"
		,"lwa","ゎ"
		,"xwa","ゎ"
		,"jyu","じゅ"
		,"zyu","じゅ"
		,"jyo","じょ"
		,"zyo","じょ"
		,"dyi","ぢぃ"
		,"dye","ぢぇ"
		,"dya","ぢゃ"
		,"dyu","ぢゅ"
		,"dyo","ぢょ"
		,"dhi","でぃ"
		,"dhe","でぇ"
		,"dha","でゃ"
		,"dhu","でゅ"
		,"dho","でょ"
		,"dwa","どぁ"
		,"dwi","どぃ"
		,"dwu","どぅ"
		,"dwe","どぇ"
		,"dwo","どぉ"
		,"byi","びぃ"
		,"bye","びぇ"
		,"bya","びょ"
		,"byu","びゅ"
		,"byo","びょ"
		,"pyi","ぴぃ"
		,"pye","ぴぇ"
		,"pya","ぴゃ"
		,"pyu","ぴゅ"
		,"pyo","ぴょ"
		,"lka","か" //ヵ
		,"xka","か" //ヵ
		,"lke","け" //ヶ
		,"xke","け" //ヶ
		,"vyi","う゛ぃ"
		,"vye","う゛ぇ"
		,"vya","う゛ゃ"
		,"vyu","う゛ゅ"
		,"vyo","う゛ょ"
		,"wu","う"
		,"la","ぁ"
		,"li","ぃ"
		,"xi","ぃ"
		,"lu","ぅ"
		,"xu","ぅ"
		,"le","ぇ"
		,"xe","ぇ"
		,"lo","ぉ"
		,"xo","ぉ"
		,"ye","いぇ"
		,"ka","か"
		,"ca","か"
		,"ki","き"
		,"qa","くぁ"
		,"qi","くぃ"
		,"qe","くぇ"
		,"qo","くぉ"
		,"ku","く"
		,"cu","く"
		,"qu","く"
		,"ke","け"
		,"ko","こ"
		,"co","こ"
		,"sa","さ"
		,"si","し"
		,"ci","し"
		,"su","す"
		,"se","せ"
		,"ce","せ"
		,"so","そ"
		,"ta","た"
		,"ti","ち"
		,"tu","つ"
		,"te","て"
		,"to","と"
		,"na","な"
		,"ni","に"
		,"nu","ぬ"
		,"ne","ね"
		,"no","の"
		,"ha","は"
		,"hi","ひ"
		,"fa","ふぁ"
		,"fa","ふぁ"
		,"fi","ふぃ"
		,"fe","ふぇ"
		,"fo","ふぉ"
		,"hu","ふ"
		,"fu","ふ"
		,"he","へ"
		,"ho","ほ"
		,"ma","ま"
		,"mi","み"
		,"mu","む"
		,"me","め"
		,"mo","も"
		,"ya","や"
		,"yu","ゆ"
		,"yo","よ"
		,"ra","ら"
		,"ri","り"
		,"ru","る"
		,"re","れ"
		,"wa","わ"
		,"wo","を"
		,"nn","ん"
		,"xn","ん"
		,"ga","が"
		,"gi","ぎ"
		,"gu","ぐ"
		,"ge","げ"
		,"go","ご"
		,"za","ざ"
		,"je","じぇ"
		,"ja","じゃ"
		,"ju","じゅ"
		,"jo","じょ"
		,"zi","じ"
		,"ji","じ"
		,"zu","ず"
		,"ze","ぞ"
		,"zo","ぞ"
		,"ji","じ"
		,"da","だ"
		,"di","ぢ"
		,"du","づ"
		,"de","で"
		,"do","ど"
		,"ba","ば"
		,"bi","び"
		,"bu","ぶ"
		,"be","べ"
		,"bo","ぼ"
		,"pa","ぱ"
		,"pi","ぴ"
		,"pu","ぷ"
		,"pe","ぺ"
		,"po","ぽ"
		,"va","う゛ぁ"
		,"vi","う゛ぃ"
		,"ve","う゛ぇ"
		,"vo","う゛ぉ"
		,"vu","う゛"
		,"a","あ"
		,"i","い"
		,"u","う"
		,"e","え"
		,"o","お"
		,NULL,NULL
	};
	static const char *replaceTableKana[] = {
		 "4@","う゛"
		,"a","あ"
		,"e","い"
		,"4","う"
		,"5","え"
		,"6","お"
		,"t","か"
		,"g","き"
		,"h","く"
		,":","け"
		,"b","こ"
		,"x","さ"
		,"d","し"
		,"r","す"
		,"p","せ"
		,"c","そ"
		,"q","た"
		,"a","ち"
		,"z","つ"
		,"w","て"
		,"s","と"
		,"u","な"
		,"i","に"
		,"1","ぬ"
		,",","ね"
		,"k","の"
		,"f","は"
		,"v","ひ"
		,"2","ふ"
		,"^","へ"
		,"-","ほ"
		,"j","ま"
		,"n","み"
		,"]","む"
		,"/","め"
		,"m","も"
		,"7","や"
		,"8","ゆ"
		,"9","よ"
		,"o","ら"
		,"l","り"
		,".","る"
		,";","れ"
		,"\\","ろ"
		,"0","わ"
		//,"","を"
		,"y","ん"
		,"#","ぁ"
		,"E","ぃ"
		,"$","ぅ"
		,"%","ぇ"
		,"&","ぉ"
		,"t@","が"
		,"g@","ぎ"
		,"h@","ぐ"
		,":@","げ"
		,"b@","ご"
		,"x@","ざ"
		,"d@","じ"
		,"r@","ず"
		,"p@","ぜ"
		,"c@","ぞ"
		,"q@","だ"
		,"a@","ぢ"
		,"z@","づ"
		,"w@","で"
		,"s@","ど"
		,"f@","ば"
		,"v@","び"
		,"2@","ぶ"
		,"^@","べ"
		,"-@","ぼ"
		,"f[","ぱ"
		,"v[","ぴ"
		,"2[","ぷ"
		,"^[","ぺ"
		,"-[","ぽ"
		,"'","ゃ"
		,"(","ゅ"
		,")","ょ"
		,"Z","っ"
		//,"ヮ","ゎ"
		,NULL,NULL
	};
	std::string ret = inTarget;
	//r	 「ローマ字」を「ひらがな」に変換します。
	//R	 「ひらがな」を「ローマ字」に変換します。
	if ( option.find("r") != -1  )
	{
		ret = XLStringUtil::replace(ret ,replaceTableRoma,true );
	}
	if ( option.find("R") != -1  )
	{
		ret = XLStringUtil::replace(ret ,replaceTableRoma,false );
	}

	//k	 「かな入力typo」を「ひらがな」に変換します。
	//K	 「ひらがな」を「かな入力typo」に変換します。
	if ( option.find("k") != -1  )
	{
		ret = XLStringUtil::replace(ret ,replaceTableKana,true );
	}
	if ( option.find("K") != -1  )
	{
		ret = XLStringUtil::replace(ret ,replaceTableKana,false );
	}
	return ret;
}



//みんな大好き PHPのmb_convert_kanaの移植
//n	 「全角」数字を「半角」に変換します。
//N	 「半角」数字を「全角」に変換します。
//a	 「全角」英数字を「半角」に変換します。
//A	 「半角」英数字を「全角」に変換します 
//s	 「全角」スペースを「半角」に変換します
//S	 「半角」スペースを「全角」に変換します（U+0020 -> U+3000）。
//k	 「全角カタカナ」を「半角カタカナ」に変換します。
//K	 「半角カタカナ」を「全角カタカナ」に変換します。
//h	 「全角ひらがな」を「半角カタカナ」に変換します。
//H	 「半角カタカナ」を「全角ひらがな」に変換します。
//c	 「全角カタカナ」を「全角ひらがな」に変換します。
//C	 「全角ひらがな」を「全角カタカナ」に変換します。
std::string XLStringUtil::mb_convert_kana(const std::string &inTarget,const std::string& option)
{
	std::string ret = inTarget;
	static const char *replaceTableAplha[] = {
		 "Ａ","A"
		,"Ｂ","B"
		,"Ｃ","C"
		,"Ｄ","D"
		,"Ｅ","E"
		,"Ｆ","F"
		,"Ｇ","G"
		,"Ｈ","H"
		,"Ｉ","I"
		,"Ｊ","J"
		,"Ｋ","K"
		,"Ｌ","L"
		,"Ｍ","M"
		,"Ｎ","N"
		,"Ｏ","O"
		,"Ｐ","P"
		,"Ｑ","Q"
		,"Ｒ","R"
		,"Ｓ","S"
		,"Ｔ","T"
		,"Ｕ","U"
		,"Ｖ","V"
		,"Ｗ","W"
		,"Ｘ","X"
		,"Ｙ","Y"
		,"Ｚ","Z"
		,"ａ","a"
		,"ｂ","b"
		,"ｃ","c"
		,"ｄ","d"
		,"ｅ","e"
		,"ｆ","f"
		,"ｇ","g"
		,"ｈ","h"
		,"ｉ","i"
		,"ｊ","j"
		,"ｋ","k"
		,"ｌ","l"
		,"ｍ","m"
		,"ｎ","n"
		,"ｏ","o"
		,"ｐ","p"
		,"ｑ","q"
		,"ｒ","r"
		,"ｓ","s"
		,"ｔ","t"
		,"ｕ","u"
		,"ｖ","v"
		,"ｗ","w"
		,"ｘ","x"
		,"ｙ","y"
		,"ｚ","z"
		,"ｰ","ー"
		,"‘","'"
		,"’","'"
		,"“","\""
		,"”","\""
		,"（","("
		,"）",")"
		,"〔","["
		,"〕","]"
		,"［","["
		,"］","]"
		,"｛","{"
		,"｝","}"
		,"〈","<"
		,"〉",">"
		,"《","<"
		,"》",">"
		,"「","{"
		,"」","}"
		,"『","{"
		,"』","}"
		,"【","["
		,"】","]"
		,"・","･"
		,"！","!"
		,"♯","#"
		,"＆","&"
		,"＄","$"
		,"？","?"
		,"：",":"
		,"；",";"
		,"／","/"
		,"＼","\\"
		,"＠","@"
		,"｜","|"
		,"－","-"
		,"＝","="
		,"≒","="
		,"％","%"
		,"＋","+"
		,"－","-"
		,"÷","/"
		,"＊","*"
		,"～","~" //UTF-8だと別の～もあるから判断が難しい・・・
		,NULL,NULL
	};
//r	 「全角」英字を「半角」に変換します。
//R	 「半角」英字を「全角」に変換します。
//a	 「全角」英数字を「半角」に変換します。
//A	 「半角」英数字を「全角」に変換します 
	if ( option.find("r") != -1 ||   option.find("a") != -1 )
	{
		ret = XLStringUtil::replace(ret ,replaceTableAplha,false );
	}
	else if ( option.find("R") != -1 ||  option.find("A") != -1 )
	{
		ret = XLStringUtil::replace(ret ,replaceTableAplha,true );
	}

	static const char *replaceTableNum[] = {
		 "１","1"
		,"２","2"
		,"３","3"
		,"４","4"
		,"５","5"
		,"６","6"
		,"７","7"
		,"８","8"
		,"９","9"
		,"０","0"
		,NULL,NULL
	};
//n	 「全角」数字を「半角」に変換します。
//N	 「半角」数字を「全角」に変換します。
//a	 「全角」英数字を「半角」に変換します。
//A	 「半角」英数字を「全角」に変換します 
	if ( option.find("n") != -1 ||  option.find("a") != -1 )
	{
		ret = XLStringUtil::replace(ret ,replaceTableNum,false );
	}
	else if ( option.find("N") != -1 ||  option.find("A") != -1)
	{
		ret = XLStringUtil::replace(ret ,replaceTableNum,true );
	}

	static const char *replaceTableSpace[] = {
		 "　"," "
		,NULL,NULL
	};
//s	 「全角」スペースを「半角」に変換します
//S	 「半角」スペースを「全角」に変換します
	if ( option.find("s") != -1 )
	{
		ret = XLStringUtil::replace(ret ,replaceTableSpace,false );
	}
	else if ( option.find("S") != -1)
	{
		ret = XLStringUtil::replace(ret ,replaceTableSpace,true );
	}

	static const char *replaceTableHankanaToHiragana[] = {
		 "ｳﾞ","う゛"
		,"ｶﾞ","が"
		,"ｷﾞ","ぎ"
		,"ｸﾞ","ぐ"
		,"ｹﾞ","げ"
		,"ｺﾞ","ご"
		,"ｻﾞ","ざ"
		,"ｼﾞ","じ"
		,"ｽﾞ","ず"
		,"ｾﾞ","ぜ"
		,"ｿﾞ","ぞ"
		,"ﾀﾞ","だ"
		,"ﾁﾞ","ぢ"
		,"ﾂﾞ","づ"
		,"ｾﾞ","ぜ"
		,"ｿﾞ","ぞ"
		,"ﾊﾞ","ば"
		,"ﾋﾞ","び"
		,"ﾌﾞ","ぶ"
		,"ﾍﾞ","べ"
		,"ﾎﾞ","ぼ"
		,"ﾊﾟ","ぱ"
		,"ﾋﾟ","ぴ"
		,"ﾌﾟ","ぷ"
		,"ﾍﾟ","ぺ"
		,"ﾎﾟ","ぽ"
		,"ｱ","あ"
		,"ｲ","い"
		,"ｳ","う"
		,"ｴ","え"
		,"ｵ","お"
		,"ｶ","か"
		,"ｷ","き"
		,"ｸ","く"
		,"ｹ","け"
		,"ｺ","こ"
		,"ｻ","さ"
		,"ｼ","し"
		,"ｽ","す"
		,"ｾ","せ"
		,"ｿ","そ"
		,"ﾀ","た"
		,"ﾁ","ち"
		,"ﾂ","つ"
		,"ﾃ","て"
		,"ﾄ","と"
		,"ﾅ","な"
		,"ﾆ","に"
		,"ﾇ","ぬ"
		,"ﾈ","ね"
		,"ﾉ","の"
		,"ﾊ","は"
		,"ﾋ","ひ"
		,"ﾌ","ふ"
		,"ﾍ","へ"
		,"ﾎ","ほ"
		,"ﾏ","ま"
		,"ﾐ","み"
		,"ﾑ","む"
		,"ﾒ","め"
		,"ﾓ","も"
		,"ﾔ","や"
		,"ﾕ","ゆ"
		,"ﾖ","よ"
		,"ﾗ","ら"
		,"ﾘ","り"
		,"ﾙ","る"
		,"ﾚ","れ"
		,"ﾛ","ろ"
		,"ｦ","を"
		,"ﾜ","わ"
		,"ﾝ","ん"
		,"ｧ","ぁ"
		,"ｨ","ぃ"
		,"ｩ","ぅ"
		,"ｪ","ぇ"
		,"ｫ","ぉ"
		,"ｬ","ゃ"
		,"ｭ","ゅ"
		,"ｮ","ょ"
		,"ｯ","っ"
		,"ｰ","ー"
		,NULL,NULL
	};
	static const char *replaceTableHankanaToKatakana[] = {
		 "ｳﾞ","ヴ"
		,"ｶﾞ","ガ"
		,"ｷﾞ","ギ"
		,"ｸﾞ","グ"
		,"ｹﾞ","ゲ"
		,"ｺﾞ","ゴ"
		,"ｻﾞ","ザ"
		,"ｼﾞ","ジ"
		,"ｽﾞ","ズ"
		,"ｾﾞ","ゼ"
		,"ｿﾞ","ゾ"
		,"ﾀﾞ","ダ"
		,"ﾁﾞ","ヂ"
		,"ﾂﾞ","ヅ"
		,"ｾﾞ","ゼ"
		,"ｿﾞ","ゾ"
		,"ﾊﾞ","バ"
		,"ﾋﾞ","ビ"
		,"ﾌﾞ","ブ"
		,"ﾍﾞ","ベ"
		,"ﾎﾞ","ボ"
		,"ﾊﾟ","パ"
		,"ﾋﾟ","ピ"
		,"ﾌﾟ","プ"
		,"ﾍﾟ","ペ"
		,"ﾎﾟ","ポ"
		,"ｱ","ア"
		,"ｲ","イ"
		,"ｳ","ウ"
		,"ｴ","エ"
		,"ｵ","オ"
		,"ｶ","カ"
		,"ｷ","キ"
		,"ｸ","ク"
		,"ｹ","ケ"
		,"ｺ","コ"
		,"ｻ","サ"
		,"ｼ","シ"
		,"ｽ","ス"
		,"ｾ","セ"
		,"ｿ","ソ"
		,"ﾀ","タ"
		,"ﾁ","チ"
		,"ﾂ","ツ"
		,"ﾃ","テ"
		,"ﾄ","ト"
		,"ﾅ","ナ"
		,"ﾆ","ニ"
		,"ﾇ","ヌ"
		,"ﾈ","ネ"
		,"ﾉ","ノ"
		,"ﾊ","ハ"
		,"ﾋ","ヒ"
		,"ﾌ","フ"
		,"ﾍ","ヘ"
		,"ﾎ","ホ"
		,"ﾏ","マ"
		,"ﾐ","ミ"
		,"ﾑ","ム"
		,"ﾒ","メ"
		,"ﾓ","モ"
		,"ﾔ","ヤ"
		,"ﾕ","ユ"
		,"ﾖ","ヨ"
		,"ﾗ","リ"
		,"ﾘ","リ"
		,"ﾙ","ル"
		,"ﾚ","レ"
		,"ﾛ","ロ"
		,"ｦ","ヲ"
		,"ﾜ","ワ"
		,"ﾝ","ン"
		,"ｧ","ァ"
		,"ｨ","ィ"
		,"ｩ","ゥ"
		,"ｪ","ェ"
		,"ｫ","ォ"
		,"ｬ","ャ"
		,"ｭ","ュ"
		,"ｮ","ョ"
		,"ｯ","ッ"
		,"ｰ","ー"
		,NULL,NULL
	};
	static const char *replaceTableKatakanaToHiragana[] = {
		 "ヴ","う゛"
		,"ア","あ"
		,"イ","い"
		,"ウ","う"
		,"エ","え"
		,"オ","お"
		,"カ","か"
		,"キ","き"
		,"ク","く"
		,"ケ","け"
		,"コ","こ"
		,"サ","さ"
		,"シ","し"
		,"ス","す"
		,"セ","せ"
		,"ソ","そ"
		,"タ","た"
		,"チ","ち"
		,"ツ","つ"
		,"テ","て"
		,"ト","と"
		,"ナ","な"
		,"ニ","に"
		,"ヌ","ぬ"
		,"ネ","ね"
		,"ノ","の"
		,"ハ","は"
		,"ヒ","ひ"
		,"フ","ふ"
		,"ヘ","へ"
		,"ホ","ほ"
		,"マ","ま"
		,"ミ","み"
		,"ム","む"
		,"メ","め"
		,"モ","も"
		,"ヤ","や"
		,"ユ","ゆ"
		,"ヨ","よ"
		,"ラ","ら"
		,"リ","り"
		,"ル","る"
		,"レ","れ"
		,"ロ","ろ"
		,"ワ","わ"
		,"ヲ","を"
		,"ン","ん"
		,"ァ","ぁ"
		,"ィ","ぃ"
		,"ゥ","ぅ"
		,"ェ","ぇ"
		,"ォ","ぉ"
		,"ガ","が"
		,"ギ","ぎ"
		,"グ","ぐ"
		,"ゲ","げ"
		,"ゴ","ご"
		,"ザ","ざ"
		,"ジ","じ"
		,"ズ","ず"
		,"ゼ","ぜ"
		,"ゾ","ぞ"
		,"ダ","だ"
		,"ヂ","ぢ"
		,"ヅ","づ"
		,"デ","で"
		,"ド","ど"
		,"バ","ば"
		,"ビ","び"
		,"ブ","ぶ"
		,"ベ","べ"
		,"ボ","ぼ"
		,"パ","ぱ"
		,"ピ","ぴ"
		,"プ","ぷ"
		,"ペ","ぺ"
		,"ポ","ぽ"
		,"ャ","ゃ"
		,"ュ","ゅ"
		,"ョ","ょ"
		,"ッ","っ"
		,"ヮ","ゎ"
		,NULL,NULL
	};
//k	 「全角カタカナ」を「半角カタカナ」に変換します。
//K	 「半角カタカナ」を「全角カタカナ」に変換します。
	if ( option.find("k") != -1 )
	{
		ret = XLStringUtil::replace(ret ,replaceTableHankanaToKatakana,true );
	}
	else if ( option.find("K") != -1)
	{
		ret = XLStringUtil::replace(ret ,replaceTableHankanaToKatakana,false );
	}

//c	 「全角カタカナ」を「全角ひらがな」に変換します。
//C	 「全角ひらがな」を「全角カタカナ」に変換します。
	if ( option.find("c") != -1 )
	{
		ret = XLStringUtil::replace(ret ,replaceTableKatakanaToHiragana,false );
	}
	else if ( option.find("C") != -1)
	{
		ret = XLStringUtil::replace(ret ,replaceTableKatakanaToHiragana,true );
	}

//h	 「全角ひらがな」を「半角カタカナ」に変換します。
//H	 「半角カタカナ」を「全角ひらがな」に変換します。
	if ( option.find("h") != -1 )
	{
		ret = XLStringUtil::replace(ret ,replaceTableHankanaToHiragana,true );
	}
	else if ( option.find("H") != -1)
	{
		ret = XLStringUtil::replace(ret ,replaceTableHankanaToHiragana,false );
	}

	return ret;
}

/*
SEXYTEST(XLStringUtil__mb_convert_kana,"XLStringUtil::mb_convert_kanaのてすと")
{
	{
//n	 「全角」数字を「半角」に変換します。
//N	 「半角」数字を「全角」に変換します。
//a	 「全角」英数字を「半角」に変換します。
//A	 「半角」英数字を「全角」に変換します 
//s	 「全角」スペースを「半角」に変換します
//S	 「半角」スペースを「全角」に変換します
//k	 「全角カタカナ」を「半角カタカナ」に変換します。
//K	 「半角カタカナ」を「全角カタカナ」に変換します。
//h	 「全角ひらがな」を「半角カタカナ」に変換します。
//H	 「半角カタカナ」を「全角ひらがな」に変換します。
//c	 「全角カタカナ」を「全角ひらがな」に変換します。
//C	 「全角ひらがな」を「全角カタカナ」に変換します。
		std::string a = XLStringUtil::mb_convert_kana("あいうえおアイウエオｱｲｳｴｵ　123１２３ ABCＡＢＣ","h"); //「全角ひらがな」を「半角カタカナ」に変換します。
		std::string b = "ｱｲｳｴｵアイウエオｱｲｳｴｵ　123１２３ ABCＡＢＣ";
		SEXYTEST_EQ(a ,b); 
	}
	{
		std::string a = XLStringUtil::mb_convert_kana("あいうえおアイウエオｱｲｳｴｵ　123１２３ ABCＡＢＣ","H"); //「半角カタカナ」を「全角ひらがな」に変換します。
		std::string b = "あいうえおアイウエオあいうえお　123１２３ ABCＡＢＣ";
		SEXYTEST_EQ(a ,b); 
	}
	{
		std::string a = XLStringUtil::mb_convert_kana("あいうえおアイウエオｱｲｳｴｵ　123１２３ ABCＡＢＣ","c"); //「全角カタカナ」を「全角ひらがな」に変換します。
		std::string b = "あいうえおあいうえおｱｲｳｴｵ　123１２３ ABCＡＢＣ";
		SEXYTEST_EQ(a ,b); 
	}
	{
		std::string a = XLStringUtil::mb_convert_kana("あいうえおアイウエオｱｲｳｴｵ　123１２３ ABCＡＢＣ","C"); //「全角ひらがな」を「全角カタカナ」に変換します。
		std::string b = "アイウエオアイウエオｱｲｳｴｵ　123１２３ ABCＡＢＣ";
		SEXYTEST_EQ(a ,b); 
	}
	{
		std::string a = XLStringUtil::mb_convert_kana("あいうえおアイウエオｱｲｳｴｵ　123１２３ ABCＡＢＣ","k"); //「全角カタカナ」を「半角カタカナ」に変換します。
		std::string b = "あいうえおｱｲｳｴｵｱｲｳｴｵ　123１２３ ABCＡＢＣ";
		SEXYTEST_EQ(a ,b); 
	}
	{
		std::string a = XLStringUtil::mb_convert_kana("あいうえおアイウエオｱｲｳｴｵ　123１２３ ABCＡＢＣ","K"); //「半角カタカナ」を「全角カタカナ」に変換します。
		std::string b = "あいうえおアイウエオアイウエオ　123１２３ ABCＡＢＣ";
		SEXYTEST_EQ(a ,b); 
	}
	{
		std::string a = XLStringUtil::mb_convert_kana("あいうえおアイウエオｱｲｳｴｵ　123１２３ ABCＡＢＣ","n"); //n	 「全角」数字を「半角」に変換します。
		std::string b = "あいうえおアイウエオｱｲｳｴｵ　123123 ABCＡＢＣ";
		SEXYTEST_EQ(a ,b); 
	}
	{
		std::string a = XLStringUtil::mb_convert_kana("あいうえおアイウエオｱｲｳｴｵ　123１２３ ABCＡＢＣ","N"); //N	 「半角」数字を「全角」に変換します。
		std::string b = "あいうえおアイウエオｱｲｳｴｵ　１２３１２３ ABCＡＢＣ";
		SEXYTEST_EQ(a ,b); 
	}
	{
		std::string a = XLStringUtil::mb_convert_kana("あいうえおアイウエオｱｲｳｴｵ　123１２３ ABCＡＢＣ","a"); //a	 「全角」英数字を「半角」に変換します。
		std::string b = "あいうえおアイウエオｱｲｳｴｵ　123123 ABCABC";
		SEXYTEST_EQ(a ,b); 
	}
	{
		std::string a = XLStringUtil::mb_convert_kana("あいうえおアイウエオｱｲｳｴｵ　123１２３ ABCＡＢＣ","A"); //A	 「半角」英数字を「全角」に変換します。
		std::string b = "あいうえおアイウエオｱｲｳｴｵ　１２３１２３ ＡＢＣＡＢＣ";
		SEXYTEST_EQ(a ,b); 
	}
	{
		std::string a = XLStringUtil::mb_convert_kana("あいうえおアイウエオｱｲｳｴｵ　123１２３ ABCＡＢＣ","s"); //s	 「全角」スペースを「半角」に変換します
		std::string b = "あいうえおアイウエオｱｲｳｴｵ 123１２３ ABCＡＢＣ";
		SEXYTEST_EQ(a ,b); 
	}
	{
		std::string a = XLStringUtil::mb_convert_kana("あいうえおアイウエオｱｲｳｴｵ　123１２３ ABCＡＢＣ","S"); //S	 「半角」スペースを「全角」に変換します
		std::string b = "あいうえおアイウエオｱｲｳｴｵ　123１２３　ABCＡＢＣ";
		SEXYTEST_EQ(a ,b); 
	}
}
*/


//みんな大好きPHPのescapeshellarg
std::string XLStringUtil::escapeshellarg(const std::string &inStr)
{
	return "\"" + replace(inStr , "\"" , "\\\"") + "\"";
}

//数字の桁数を求める
int XLStringUtil::getScaler(unsigned int num)
{
	if (num < 10) return 1;
	else if (num < 100) return 2;
	else if (num < 1000) return 3;
	else if (num < 10000) return 4;
	else if (num < 100000) return 5;
	else if (num < 1000000) return 6;
	else if (num < 10000000) return 7;
	else if (num < 100000000) return 8;
	else if (num < 1000000000) return 9;
	else return 10;
}

