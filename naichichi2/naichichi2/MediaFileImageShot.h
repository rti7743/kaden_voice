#pragma once

#include <string>
#include <shlobj.h>

//ファイル情報の解析.
class MediaFileImageShot
{
public:
	MediaFileImageShot::MediaFileImageShot();
	virtual MediaFileImageShot::~MediaFileImageShot();
	xreturn::r<bool> MediaFileImageShot::Create(MainWindow* poolMainWindow);

	xreturn::r<bool> MediaFileImageShot::Shot(const std::string& dir,const std::string& filename ,std::vector<char> * image );


private:
	//ファイル名からLPITEMIDLISTを何とかして求めます。
	xreturn::r<bool> MediaFileImageShot::ConvertFileNameToPIDL(const std::string& fullpath,LPITEMIDLIST* lpcItemList) const;
	//HBITMAPを *.jpeg形式でメモリに保存
	xreturn::r<bool> MediaFileImageShot::ConvertHBITMAPtoJPEGByte(HBITMAP hbitmap,std::vector<char> * image) const;
	//HICONをjpegで保存します
	xreturn::r<bool> MediaFileImageShot::ConvertHICONtoBytes(HICON hicon,std::vector<char> * image) const;

};
