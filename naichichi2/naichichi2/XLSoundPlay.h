#pragma once

typedef void (*XLSoundPlay_CALLBACK)(int _this, int event);

class IXLSoundPlay_Interface
{
public:
	virtual bool play(const std::string & filename) = 0;
};

//なんとかしてmp3を再生する.
class XLSoundPlay
{
public:
	XLSoundPlay();
	virtual ~XLSoundPlay();

	bool play(const std::string & filename) ;
private:
	IXLSoundPlay_Interface* Object;
};
