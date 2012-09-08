// XLImage.cpp: XLImage クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "common.h"
#include "XLImage.h"
#include "XLStringUtil.h"
#include <atlbase.h>

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

XLImage::XLImage()
{
	this->image = NULL;
}
XLImage::XLImage(Gdiplus::Image* newImage)
{
	this->image = newImage;
}

XLImage::~XLImage()
{
	Clear();
}
void XLImage::Clear()
{
	if (this->image)
	{
		delete this->image;
		this->image = NULL;
	}
}

//画像読み込み
bool XLImage::Load(const std::string & fileName)
{
	_USE_WINDOWS_ENCODING;

	Clear();
	this->image = Gdiplus::Bitmap::FromFile(_A2W(fileName.c_str()) , TRUE);
	if (!this->image)
	{
		throw XLException(std::string() + "ファイル" + fileName + "の読み込みに失敗しました");
	}
	return true;
}

//画像読み込み
bool XLImage::Load(const std::vector<char> & data)
{
	Clear();
	//GlobalFreeは IStream が行う。
	HGLOBAL memoryHandle = GlobalAlloc(GMEM_MOVEABLE, data.size());
	//転送
	{
		void * memory = GlobalLock(memoryHandle);
		memcpy(memory , &data[0] , data.size() );
		GlobalUnlock(memory);
	}

	//メモリからストリーム作成
	CComPtr<IStream> iStream = NULL;
	HRESULT hr = CreateStreamOnHGlobal(memoryHandle, TRUE, &iStream);
	if(FAILED(hr))	 throw XLException(StringWindows(hr));
	

	this->image = Gdiplus::Bitmap::FromStream(iStream , TRUE);
	if (!this->image)
	{
		throw XLException(std::string() + "メモリから画像の読み込みに失敗しました");
	}
	return true;
}

//画像読み込み
bool XLImage::Load(HBITMAP hbitmap)
{
	Clear();
	this->image = Gdiplus::Bitmap::FromHBITMAP(hbitmap,NULL);
	if (!this->image)
	{
		throw XLException(std::string() + "HBITMAPからの読み込みに失敗しました");
	}
	return true;
}

//画像読み込み
bool XLImage::Load(HICON hicon)
{
	Clear();
	this->image = Gdiplus::Bitmap::FromHICON(hicon);
	if (!this->image)
	{
		throw XLException(std::string() + "HBITMAPからの読み込みに失敗しました");
	}
	return true;
}

//画像保存
bool XLImage::Save(const std::string & fileName,int option) const
{
	_USE_WINDOWS_ENCODING;

	assert(IsEnable()) ;
	std::string ext = XLStringUtil::strtolower(XLStringUtil::baseext(fileName));

	CLSID clsid;
	auto r1 = findEncoder(ext,&clsid);
	if (!r1)
	{
		throw XLException(r1.getError());
	}


	Gdiplus::Status status;
	//メモリ内に保存
	if (ext == ".jpeg" || ext == ".jpg")
	{
		//圧縮率を指定するパラメーターを作る
		Gdiplus::EncoderParameters params = {0};
		params.Count						= 1;
		params.Parameter[0].Guid			= Gdiplus::EncoderQuality;
		params.Parameter[0].Type			= Gdiplus::EncoderParameterValueTypeLong;
		params.Parameter[0].NumberOfValues	= 1;
		params.Parameter[0].Value			= &option;

		status = this->image->Save(_A2W(fileName.c_str()) , &clsid ,&params);
	}
	else
	{
		status = this->image->Save(_A2W(fileName.c_str()) , &clsid);
	}
	if (status != Gdiplus::Ok)
	{
		throw XLException("保存に失敗しました");
	}
	return true;
}

//画像保存
bool XLImage::Save(const std::string & ext,std::vector<char> * data,int option) const
{
	assert(IsEnable()) ;

	CLSID clsid;
	findEncoder(ext,&clsid);

//bitmapで保持した時の最大数を割り振るとだめっぽい
//	const int imagesize = bitmap->GetWidth() * bitmap->GetHeight() * 24;
	const int imagesize = 1;  //最低限の数字にする。割り当てると、後でサイズを要求した時に、割り当てたサイズを返すみたい。
	//GlobalFreeは IStream が行う。
	HGLOBAL memoryHandle = GlobalAlloc(GMEM_MOVEABLE, imagesize);

	//メモリからストリーム作成
	CComPtr<IStream> iStream = NULL;
	HRESULT hr = CreateStreamOnHGlobal(memoryHandle, TRUE, &iStream);
	if(FAILED(hr))	 throw XLException(StringWindows(hr));

	Gdiplus::Status status;
	//メモリ内に保存
	if (ext == ".jpeg" || ext == ".jpg")
	{
		//圧縮率を指定するパラメーターを作る
		Gdiplus::EncoderParameters params = {0};
		params.Count						= 1;
		params.Parameter[0].Guid			= Gdiplus::EncoderQuality;
		params.Parameter[0].Type			= Gdiplus::EncoderParameterValueTypeLong;
		params.Parameter[0].NumberOfValues	= 1;
		params.Parameter[0].Value			= &option;

		status = this->image->Save(iStream , &clsid ,&params);
	}
	else
	{
		status = this->image->Save(iStream , &clsid);
	}
	if (status != Gdiplus::Ok)
	{
		throw XLException("保存に失敗しました");
	}
	//サイズを取得します。
	STATSTG statstg;
	hr = iStream->Stat(&statstg ,0);
	if(FAILED(hr))	 throw XLException(StringWindows(hr));
	//バッファ長確保して代入します。
	data->resize(  statstg.cbSize.LowPart);

	const void * memory = GlobalLock(memoryHandle);
	memcpy(&(data->operator[](0)) , memory , statstg.cbSize.LowPart );
	GlobalUnlock(memoryHandle);	

	return true;
}

//描画
void XLImage::Draw(XLImage* image,int x,int y) const
{
	assert(IsEnable()) ;
	assert(image->IsEnable()) ;

	Gdiplus::Graphics graphics(this->image);
	graphics.DrawImage(this->image ,x ,y);
}

//描画
void XLImage::Draw(XLImage* image,int x,int y,int width,int height) const
{
	assert(IsEnable()) ;
	assert(image->IsEnable()) ;

	Gdiplus::Graphics graphics(this->image);
	graphics.DrawImage(this->image ,x ,y,width,height);
}


//描画
void XLImage::Draw(HDC dc,int x,int y) const
{
	assert(IsEnable()) ;

	Gdiplus::Graphics graphics(dc);
	graphics.DrawImage(this->image ,x ,y);
}
//描画
void XLImage::Draw(HDC dc,int x,int y,int width,int height) const
{
	Gdiplus::Graphics graphics(dc);
	graphics.DrawImage(this->image ,x ,y,width,height);
}

//変形
Gdiplus::Image* XLImage::GetThumbnailImage(int width,int height) 
{
	return this->image->GetThumbnailImage(width,height,NULL,NULL);
}



bool XLImage::findEncoder(const std::string & ext,CLSID* clsid) const
{
	const wchar_t * format;
	if (ext == ".jpeg" || ext == ".jpg")
	{
		format = L"image/jpeg";
	}
	else if (ext == ".png")
	{
		format = L"image/png";
	}
	else if (ext == ".gif")
	{
		format = L"image/gif";
	}
	else if (ext == ".bmp")
	{
		format = L"image/bmp";
	}
	else
	{
		throw XLException(std::string() + "指定された拡張子" + ext +"に関連付けられたエンコーダーはありません");
	}
	
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes
	Gdiplus::GetImageEncodersSize(&num, &size);
	if(size == 0)
	{
		throw XLException("Gdiplus::GetImageEncodersSizeに失敗");
	}

	Gdiplus::ImageCodecInfo* pImageCodecInfo =
				(Gdiplus::ImageCodecInfo*)(malloc(size));
	if(pImageCodecInfo == NULL)
	{
		throw XLException("Gdiplus::ImageCodecInfo のメモリ確保に失敗");
	}

	Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

	UINT j = 0;
	for(; j < num; ++j)
	{
		if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
		{
			*clsid = pImageCodecInfo[j].Clsid;
			return true;
		}    
	}

	free(pImageCodecInfo);
	throw XLException("エンコーダが見つかりません");
}


/*
//HBITMAPを *.bmp形式でメモリに保存
bool XLImage::ConvertHBITMAPtoBytes(HBITMAP hbitmap,std::vector<char> * image) const
{
	BITMAP bmp; 
	// Retrieve the bitmap color format, width, and height. 
	if (!GetObject(hbitmap, sizeof(BITMAP), (LPSTR)&bmp)) 
	{
		throw XLException("HBITMAP から BITMAPに変換できません");
	}
 
	// Convert the color format to a count of bits. 
	WORD cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel); 
	if (cClrBits == 1)			cClrBits = 1; 
	else if (cClrBits <= 4)		cClrBits = 4; 
	else if (cClrBits <= 8)		cClrBits = 8; 
	else if (cClrBits <= 16)	cClrBits = 16; 
	else if (cClrBits <= 24)	cClrBits = 24; 
	else						cClrBits = 32; 
 
	BITMAPINFOHEADER bitmapInfoHeader ={0};
	PBITMAPINFO pbmi = (PBITMAPINFO)&bitmapInfoHeader;

	// Initialize the fields in the BITMAPINFO structure. 
 	pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER); 
	pbmi->bmiHeader.biWidth = bmp.bmWidth; 
	pbmi->bmiHeader.biHeight = bmp.bmHeight; 
	pbmi->bmiHeader.biPlanes = bmp.bmPlanes; 
	pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel; 
	if (cClrBits < 24) pbmi->bmiHeader.biClrUsed = (1<<cClrBits); 
 
	// If the bitmap is not compressed, set the BI_RGB flag. 
	pbmi->bmiHeader.biCompression = BI_RGB; 
 
	// Compute the number of bytes in the array of color 
	// indices and store the result in biSizeImage. 
	// For Windows NT, the width must be DWORD aligned unless 
	// the bitmap is RLE compressed. This example shows this. 
	// For Windows 95/98/Me, the width must be WORD aligned unless the 
	// bitmap is RLE compressed.
	pbmi->bmiHeader.biSizeImage = ((pbmi->bmiHeader.biWidth * cClrBits +31) & ~31) / 8 * pbmi->bmiHeader.biHeight; 

	// Set biClrImportant to 0, indicating that all of the 
	// device colors are important. 
	pbmi->bmiHeader.biClrImportant = 0; 

	//出力先メモリの確保
	const int totalsize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + pbmi->bmiHeader.biSizeImage;
	image->resize(totalsize);
	char * writeMemory = &(image->operator[](0));

	PBITMAPINFOHEADER pbih = (PBITMAPINFOHEADER) pbmi; 


	BITMAPFILEHEADER* hdr = (BITMAPFILEHEADER*)writeMemory;       // bitmap file-header 
	hdr->bfType = 0x4d42;        // 0x42 = "B" 0x4d = "M" 
	// Compute the size of the entire file. 
	hdr->bfSize = (DWORD) (sizeof(BITMAPFILEHEADER) + pbih->biSize + pbih->biClrUsed * sizeof(RGBQUAD) + pbih->biSizeImage); 
	hdr->bfReserved1 = 0; 
	hdr->bfReserved2 = 0; 
 
	// Copy the BITMAPINFOHEADER and RGBQUAD array into the file.
	memcpy( writeMemory+sizeof(BITMAPFILEHEADER) , pbih , sizeof(BITMAPINFOHEADER) );

	// Copy the array of color indices into the .BMP file.
	hdr->bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) + pbih->biSize + pbih->biClrUsed * sizeof (RGBQUAD); 
 
	// Retrieve the color table (RGBQUAD array) and the bits 
	// (array of palette indices) from the DIB. 
	HDC dc = GetDC(NULL);
	LPBYTE lpBits = (LPBYTE) writeMemory + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	if (!GetDIBits(dc, hbitmap, 0, (WORD) pbih->biHeight, lpBits, pbmi, DIB_RGB_COLORS)) 
	{
		auto error = ::GetLastError();
		ReleaseDC(NULL,dc);
		throw XLException(StringWindows(error));
	}
	ReleaseDC(NULL,dc);

	return true;
}
*/
