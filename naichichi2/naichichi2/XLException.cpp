#include "common.h"
#include "XLException.h"

std::string XLException::StringErrNo()
{
	return StringErrNo(errno);
}
std::string XLException::StringWindows()
{
	return StringWindows(::GetLastError());
}

std::string XLException::StringErrNo(int errorno)
{
	std::stringstream out;
	out << "errno:" << errorno << " info:";
#ifdef __STDC_WANT_SECURE_LIB__
		char buffer[1024];
		strerror_s( buffer , sizeof(buffer)-1, errorno);
		buffer[sizeof(buffer)-1] = '\0';
		out << buffer;
#else
		out << strerror( errorno );
#endif
		return out.str();
}
std::string XLException::StringWindows(unsigned long errorno )
{
	std::stringstream out;
	out << "GetLastError:" << errorno << " info:";

	void* msgBuf;
	::FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		errorno,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // デフォルト言語
		(LPTSTR) &msgBuf,
		0,
		NULL 
	);
	if (msgBuf == NULL)
	{
		out << "can not found error!";
	}
	else
	{
		out << msgBuf;
	}
	return out.str();
}


