#include "common.h"
#include "XLSoundPlay.h"
#include "XLStringUtil.h"



#ifdef _MSC_VER
#pragma comment(lib,"winmm.lib")

//MCIを利用してmp3を再生する.
//http://www5b.biglobe.ne.jp/~u-hei/memorandom/aviplaybackfrommemory/step1_mci.html
//http://programeasy.soudesune.net/winapi/35.html
class XLSoundPlay_MCI : public IXLSoundPlay_Interface
{
public:
	XLSoundPlay_MCI()
	{
	}
	virtual ~XLSoundPlay_MCI()
	{
	}

	virtual bool play(const std::string & filename)
	{
		std::string ext = XLStringUtil::strtolower(XLStringUtil::baseext(filename));

		MCI_OPEN_PARMS open_parms;
		MCI_PLAY_PARMS play_parms;
		MCI_GENERIC_PARMS parms;
		open_parms.wDeviceID = 0;
		open_parms.lpstrElementName = filename.c_str();
		if (ext == ".mp3") 
		{
			open_parms.lpstrDeviceType = "MPEGVideo";
		}
		else if (ext == ".wav")
		{
			open_parms.lpstrDeviceType = "WaveAudio";
		}
		else
		{
			throw XLException("未対応の形式" + ext + "が選択されました。 ファイル:" + filename );
		}

		if(mciSendCommand(0,MCI_OPEN,MCI_WAIT|MCI_OPEN_TYPE|MCI_OPEN_ELEMENT,(DWORD)&open_parms) == 0){
			play_parms.dwFrom = 0;
			mciSendCommand(open_parms.wDeviceID,MCI_PLAY,MCI_WAIT|MCI_FROM,(DWORD)&play_parms);
			mciSendCommand(open_parms.wDeviceID,MCI_STOP,MCI_WAIT,(DWORD)&parms);
		}

		return true;
	}
};
#else
class XLSoundPlay_MPG123 : public IXLSoundPlay_Interface
{
public:
	XLSoundPlay_MPG123()
	{
	}
	virtual ~XLSoundPlay_MPG123()
	{
	}

	virtual bool play(const std::string & filename)
	{
		std::string command = std::string("mpg123 ") + "\"" + filename + "\""; //!!
		system(command.c_str());
		return true;
	}
};
#endif


//なんとかして音楽(wav/mp3も)を再生する.
XLSoundPlay::XLSoundPlay()
{
#ifdef _MSC_VER
	this->Object = new XLSoundPlay_MCI;
#else
	this->Object = new XLSoundPlay_MPG123;
#endif
}

XLSoundPlay::~XLSoundPlay()
{
	delete this->Object;
}

bool XLSoundPlay::play(const std::string & filename) 
{
	return this->Object->play(filename);
}

