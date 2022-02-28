#include "PictureFile.h"
#include "Picture.h"
#include "String/String.h"
#include "System.h"

#include <gdiplus.h>

#pragma comment (lib,"Gdiplus.lib")

PictureFile::PictureFile() {

}

PictureFile::~PictureFile() {

}

bool PictureFile::CreatePicture(Picture& picture, const WString& filename) {
	static GDIPictureFile pictureFile;
	if (!pictureFile.Create(picture, filename)) {
		return false;
	}
	return true;
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