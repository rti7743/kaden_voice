#include <functional>
#include <strstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <time.h>



namespace xlogwriter
{ 
	enum XLOGLEVEL 
	{
		 DEBUG = 0
		,NOTIFY = 10
		,WARNING = 20
		,ERROR = 30
	};
	XLOGLEVEL loglevel = XLOGLEVEL::NOTIFY;

	//ログのprefixを定義する.
	//ディフォルトは YYYY/MM/DD\tHH:MM:SS\t
	std::function< std::string() > prefix_function = [](){
		return datetime();
	};
	//ログセパレーター
	std::function< std::string() > separator_function = [](){
		return "\t";
	};
	//出力するターゲットの名前 (ファイルの場合ファイル名)
	std::function< std::string() > outputtarget_function = [](){
		return "log.log";
	};
	//出力方法を定義する.
	std::function< void(const std::string & str) > write_function = [](const std::string & str){
		std::string filename = outputtarget_function();

		std::ostrstream file_header_stream(filename);
		file_header_stream << str;
	};

	std::string date()
	{
		time_t now;
		time(&now);

		struct tm date;
		localtime_s(&date, &now);

		char buffer[20];
		std::stringstream str(buffer,sizeof(buffer));

		str.fill('0');
		str 
			<< date.tm_year + 1900
			<< std::setw(2) << date.tm_mon + 1
			<< std::setw(2) << date.tm_mday + 1
			;
	}
	std::string datetime()
	{
		time_t now;
		time(&now);

		struct tm date;
		localtime_s(&date, &now);

		char buffer[20];
		std::stringstream st(buffer,sizeof(buffer));

		st.fill('0');
		st 
			<< date.tm_year + 1900
			<< std::setw(2) << date.tm_mon + 1
			<< std::setw(2) << date.tm_mday + 1
			<< "-"
			<< std::setw(2) << date.tm_hour
			<< std::setw(2) << date.tm_min
			<< std::setw(2) << date.tm_sec
			;
		return st.str();
	}

	class logwriter : public std::stringstream
	{
	public:
		logwriter()
		{
			*this << prefix_function() << separator_function();
		}
		virtual ~logwriter()
		{
			write_function( this->str() );
		}
	};
};


#define XLOG_DEBUG(EXPR) if (xlogwriter::loglevel >= xlogwriter::XLOGLEVEL::DEBUG) xlogwriter::xlogwriter << EXPR;
#define XLOG_NOTIFY(EXPR) if (xlogwriter::loglevel >= xlogwriter::XLOGLEVEL::NOTIFY) xlogwriter::xlogwriter << EXPR;
#define XLOG_WARNING(EXPR) if (xlogwriter::loglevel >= xlogwriter::XLOGLEVEL::WARNING) xlogwriter::xlogwriter << EXPR;
#define XLOG_ERROR(EXPR) if (xlogwriter::loglevel >= xlogwriter::XLOGLEVEL::ERROR) xlogwriter::xlogwriter << EXPR;
