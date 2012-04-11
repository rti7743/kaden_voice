#pragma once

typedef void (*XLSoundPlay_CALLBACK)(int _this, int event);

class IXLSoundPlay_Interface
{
public:
	virtual xreturn::r<bool> play(const std::string & filename) = 0;
};

//‚È‚ñ‚Æ‚©‚µ‚Ämp3‚ğÄ¶‚·‚é.
class XLSoundPlay
{
public:
	XLSoundPlay();
	virtual ~XLSoundPlay();

	xreturn::r<bool> play(const std::string & filename) ;
private:
	IXLSoundPlay_Interface* Object;
};
