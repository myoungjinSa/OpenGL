#include "PictureFile.h"
#include "String/String.h"
#include "System.h"
#include "VideoFile.h"

#include <gdiplus.h>

#pragma comment (lib,"Gdiplus.lib")

WString imageList[]{
	L".png",
	L".bmp",
	L".jpg",
	L".jpeg",
	L".tga",
	L".tiff",
	L".tif"
};

WString videoList[]{
	L".mxf",
	L".mov",
	L".mp4",
};

PictureFile::PictureFile() {

}

PictureFile::~PictureFile() {

}

bool PictureFile::CreateVideoFrame(Picture& picture, const WString& filename, int frameNo) {
	//std::unique_ptr<VideoFile> VideoReader =  

	return true;
}

bool PictureFile::CreatePicture(Picture& picture, const WString& filename) {
	static GDIPictureFile pictureFile;
	if (!pictureFile.Create(picture, filename)) {
		return false;
	}
	return true;
}

bool PictureFile::IsVideoFile(const WString& filename) {
	WString Extent = FileUtils::GetFileExtent(filename);
	if (Extent.Empty())
		return false;
	
	for (size_t iVideoFormat = 0; iVideoFormat < _countof(videoList); iVideoFormat++) {
		if (Extent.CompareNoCase(videoList[iVideoFormat])) {
			return true;
		}
	}
	return true;
}

bool PictureFile::IsImageFile(const WString& filename) {
	WString Extent = FileUtils::GetFileExtent(filename);
	if (Extent.Empty())
		return false;

	for (size_t iImageFormat = 0; iImageFormat < _countof(imageList); iImageFormat++) {
		if (Extent.CompareNoCase(imageList[iImageFormat])) {
			return true;
		}
	}
	return false;
}

unsigned __int64 GDIPictureFile::gdiplusToken = 0;

bool GDIPictureFile::LoadGDIPlus() {
	Gdiplus::GdiplusStartupInput gdiplusStartupInput(NULL, 0, 0);
	Gdiplus::Status result = Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, 0);
	assert(result == Gdiplus::Ok);
	return result;
}

void GDIPictureFile::UnloadGDIPlus() {
	Gdiplus::GdiplusShutdown(gdiplusToken);
}
GDIPictureFile::GDIPictureFile() 
	:PictureFile()
{
	LoadGDIPlus();
}

GDIPictureFile::~GDIPictureFile() {
	UnloadGDIPlus();
}
bool GDIPictureFile::Create(Picture& picture, const WString& filename) {
	Gdiplus::Bitmap bitmap(filename.c_str(), FALSE);
	Gdiplus::Status result = bitmap.GetLastStatus();
	if (result != Gdiplus::Ok) {
		return false;
	}

	if (!picture.Create(nullptr, bitmap.GetWidth(), bitmap.GetHeight(), DEFAULT_BPP)) {
		return false;
	}

	Gdiplus::BitmapData bmpData;
	bool isAlpha = Gdiplus::IsAlphaPixelFormat(bitmap.GetPixelFormat());
	
	bmpData.Width = picture.GetWidth();
	bmpData.Height = picture.GetHeight();
	bmpData.Stride = picture.GetPitch();
	bmpData.Scan0 = picture.GetMemory();
	bmpData.PixelFormat = isAlpha ? PixelFormat32bppARGB : PixelFormat24bppRGB;
	bmpData.Reserved = 0;

	Gdiplus::Rect lockRect(0, 0, bitmap.GetWidth(), bitmap.GetHeight());
	if (Gdiplus::Ok != bitmap.LockBits(&lockRect, Gdiplus::ImageLockModeRead | Gdiplus::ImageLockModeUserInputBuf, PixelFormat32bppARGB, &bmpData))
		return false;

	bitmap.UnlockBits(&bmpData);
	return true;
}

bool GDIPictureFile::Load(Picture& picture, const WString& filename) {
	return true;
}

bool GDIPictureFile::Save(Picture& picture, const WString& filename) {
	
	return true;
} 


VideoInfo::VideoInfo()
	: imageSize(0, 0)
	, pixelFormat(ePixelFormat::PIXEL_FORMAT_ARGB)
	, colorSpace(eColorSpace::COLOR_SPACE_DEFAULT)
	, bpp(DEFAULT_BPP)
	, startFrameNo(0)
	, frameCount(1)
{
	
}