#include "common.h"
#include "ActionImplement.h"
#include "XLHttpRequerst.h"
#include "XLStringUtil.h"

ActionImplement::ActionImplement(void)
{
}


ActionImplement::~ActionImplement(void)
{
}

xreturn::r<std::string> ActionImplement::Telnet(const std::string& host,int port,const std::string& wait,const std::string& send,const std::string& recv)
{
	//ASIO作成
	boost::asio::io_service io_service;

	 //名前解決
	boost::asio::ip::tcp::resolver resolver(io_service);
	boost::asio::ip::tcp::resolver::query query(host , num2str( port ));
	boost::asio::ip::tcp::endpoint endpoint(*resolver.resolve(query));

    boost::asio::ip::tcp::socket socket(io_service);

	//接続
	boost::system::error_code error ;
	socket.connect(endpoint,error);
    if (error)
	{
		return xreturn::error("ホストに接続できません host:" + host + " port:" + num2str(port) );
	}

	//まずは相手の言い分を聞きます.
	if (!wait.empty())
	{
		boost::asio::streambuf wait_body;
		boost::asio::read_until(socket, wait_body,wait ,error );
	}

	//送信
	if (!send.empty() )
	{
		boost::asio::streambuf request;
		std::ostream request_stream(&request);
		request_stream << send << std::flush;
		boost::asio::write(socket, request);
	}

	//最後に会いての言い分を聞いてから通信を切ります。
	if (!recv.empty())
	{
		//貴方の意見は？
		boost::asio::streambuf respons_body;
		boost::asio::read_until(socket, respons_body,recv ,error );
		return boost::asio::buffer_cast<const char*>(respons_body.data());
	}
	return "";
}

xreturn::r<std::string> ActionImplement::HttpGet(const std::string& url)
{
	XLHttpRequerst s;
	std::map<std::string,std::string> header;
	std::map<std::string,std::string> option;
	return s.GetContents(url ,header,option );
}

xreturn::r<std::string> ActionImplement::HttpPost(const std::string& url,const std::string& postdata)
{
	XLHttpRequerst s;
	std::map<std::string,std::string> header;
	std::map<std::string,std::string> option;
	option["method"] = "POST";
	option["data"] = postdata;

	return s.GetContents(url ,header,option );
}

xreturn::r<bool> ActionImplement::Execute(const std::string& targetpc,const std::string& command,const std::string& args,const std::string& directory)
{
	std::string _command = XLStringUtil::pathcombine(directory , command);
	std::string _args = XLStringUtil::pathseparator(args);

	::ShellExecute(NULL,"open",_command.c_str(),_args.c_str(),NULL,SW_SHOWNORMAL);

	//成功失敗の判定がうまくいかない時があるので、とりあえずこれで。
	return true;
}

xreturn::r<bool> ActionImplement::SendKeydown(const std::string& targetpc,const std::string& windowname,int key , int keyoption)
{
	HWND hwnd = ::GetTopWindow(::GetDesktopWindow());
	do
	{
		char title[ MAX_PATH ]= {0};
		::GetWindowText( hwnd, title, sizeof(title) );

		if ( strstr(title , windowname.c_str() ) == NULL )
		{
			continue;
		}
		
		//keybd_event
		if (keyoption & 1)  ::SendMessage(hwnd , WM_KEYDOWN ,VK_CONTROL ,   0 );
		if (keyoption & 2)  ::SendMessage(hwnd , WM_KEYDOWN ,VK_LMENU     ,   0 );
		if (keyoption & 4)  ::SendMessage(hwnd , WM_KEYDOWN ,VK_LSHIFT  ,   0 );
		::SendMessage(hwnd , WM_KEYDOWN ,key, 0 );
		::SendMessage(hwnd , WM_CHAR    ,key, 0 );
		::SendMessage(hwnd , WM_KEYUP   ,key, 0 );
		if (keyoption & 4)  ::SendMessage(hwnd , WM_KEYUP ,VK_LSHIFT  ,   0 );
		if (keyoption & 2)  ::SendMessage(hwnd , WM_KEYUP ,VK_LMENU     ,   0 );
		if (keyoption & 1)  ::SendMessage(hwnd , WM_KEYUP ,VK_CONTROL ,   0 );
	}
	while( (hwnd = ::GetNextWindow(hwnd,GW_HWNDNEXT)) !=NULL );

	return true;
}

xreturn::r<bool> ActionImplement::SendMessage(const std::string& targetpc,const std::string& windowname,int message,int wparam,int  lparam)
{
	HWND hwnd = ::GetTopWindow(::GetDesktopWindow());
	do
	{
		char title[ MAX_PATH ]= {0};
		::GetWindowText( hwnd, title, sizeof(title) );

		if ( strstr(title , windowname.c_str() ) == NULL )
		{
			continue;
		}
		
		//keybd_event
		::SendMessage(hwnd , message , wparam,lparam );
	}
	while( (hwnd = ::GetNextWindow(hwnd,GW_HWNDNEXT)) !=NULL );

	return true;
}

xreturn::r<bool> ActionImplement::MSleep(const std::string& targetpc,unsigned int mtime )
{
	const unsigned int sleep_poolingtime = 100;
	unsigned int loop = mtime / sleep_poolingtime;
	if (loop <= 0) loop ++;
	//この実装はおろかだけど、とりあえずこれで。
	//main thread に対する停止や linux対応、などを考えるとまずはこれで様子を見る.
	for(unsigned int t = 0 ; t < loop ; ++t)
	{
//		if (*threadStopFlag)
//		{
//			return false;
//		}
		::Sleep(sleep_poolingtime);
	}
	return true;
}
