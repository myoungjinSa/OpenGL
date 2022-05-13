#include "PictureFile.h"
#include "String/String.h"
#include "System.h"
#include "FFMPEGDecoder.h"

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
	L".webm"
};


std::unique_ptr<VideoFile> PictureFile::DefaultFactory::CreateVideoFile(const WString& filename) {
	if (filename.Empty())
		return nullptr;

	if (!IsVideoFile(filename))
		return nullptr;

	return std::make_unique<FFMPGVideoReader>();
}

PictureFile::DefaultFactory PictureFile::FactoryPool::defaultFactory;
PictureFile::FactoryPool::FactoryPool() 
	: pCodecLimits(nullptr)
{
}

PictureFile::FactoryPool::~FactoryPool() {
	Clear();
}

void PictureFile::FactoryPool::Clear() {
	factoryCandidates.clear();
}


bool PictureFile::FactoryPool::Add(std::unique_ptr<Factory> _factory) {
	factoryCandidates.push_back(std::move(_factory));
	return true;
}

void PictureFile::FactoryPool::Remove(Factory& factory) {
	std::vector<std::unique_ptr<Factory>>::iterator iFound = std::find_if(factoryCandidates.begin(), factoryCandidates.end(), [&](std::unique_ptr<Factory>& _factory) {
		return &factory == _factory.get();
		}
	);
	factoryCandidates.erase(std::remove(factoryCandidates.begin(), factoryCandidates.end(), *iFound));
}

size_t PictureFile::FactoryPool::GetCount() const {
	return factoryCandidates.size();
}

bool PictureFile::FactoryPool::IsSupported(const Codec& codec) const{
	if (pCodecLimits && !pCodecLimits->IsSupported(codec)) {
		return false;
	}
	return true;
}

std::unique_ptr<VideoFile> PictureFile::FactoryPool::CreateVideoFile(const WString& filename) {
	for (size_t iFactory = 0; iFactory < factoryCandidates.size(); iFactory++) {
		Factory* pFactory = factoryCandidates[iFactory].get();
		if (pFactory) {
			std::unique_ptr<VideoFile> VideoFile = pFactory->CreateVideoFile(filename);
			if (VideoFile != nullptr) {
				return std::move(VideoFile);
			}
		}
	}
	return defaultFactory.CreateVideoFile(filename);
}

std::unique_ptr<VideoFile> PictureFile::FactoryPool::CreateVideoFile(const WString& filename, const Codec& codec) {
	for (size_t iFactory = 0; iFactory < factoryCandidates.size(); iFactory++) {
		Factory* pFactory = factoryCandidates[iFactory].get();
		if (pFactory) {
			const std::shared_ptr<CodecLimits> codecLimits = pFactory->GetCodecLimits();
			{
				if (!codecLimits->IsSupported(codec))
					continue;
			}
			std::unique_ptr<VideoFile> VideoFile = pFactory->CreateVideoFile(filename, codec);
			if (VideoFile != nullptr)
				return std::move(VideoFile);
		}
	}
	return defaultFactory.CreateVideoFile(filename);
}


PictureFile::FactoryPool PictureFile::factoryPool;

PictureFile::PictureFile() {

}

PictureFile::~PictureFile() {

}

bool PictureFile::AddFactory(std::unique_ptr<Factory> factory) {
	if (factory != nullptr) {
		return false;
	}
	return factoryPool.Add(std::move(factory));
}

void PictureFile::RemoveFactory(Factory& _factory) {
	factoryPool.Remove(_factory);
}

void PictureFile::ClearFactory() {
	factoryPool.Clear();
}

void PictureFile::SetCodecLimits(std::shared_ptr<CodecLimits>& codecLimits) {
	factoryPool.SetCodecLimits(codecLimits);
}

bool PictureFile::IsSupportedCodec(const Codec& codec) {
	if (factoryPool.codecLimits != nullptr) {
		if (!factoryPool.codecLimits->IsSupported(codec))
			return false;
	}

	return factoryPool.IsSupported(codec);
}


bool PictureFile::CreateVideoFrame(Picture& picture, std::unique_ptr<VideoFile>& _videoFile, const WString& filename, int64_t frameNo) {
	bool bLoaded = false;
	if (std::unique_ptr<VideoFile> videoFile = factoryPool.CreateVideoFile(filename)) {
		if (!videoFile->OpenFile(filename, ePixelFormat::PIXEL_FORMAT_ARGB))
			return false;

		if (videoFile->IsAudioValid()) {
			videoFile->InitAudioDevice();
		}
		if (UpdateVideoFrame(picture, videoFile, frameNo)) {
			bLoaded = true;
		}
		if (videoFile == nullptr) {
			videoFile->Close();
		}

		_videoFile = std::move(videoFile);
	}

	return bLoaded;
}

bool PictureFile::SeekVideoFrame(Picture& picture, std::unique_ptr<VideoFile>& videoFile, int64_t _pts) {
	if (!videoFile->IsOpened())
		return false;

	VideoAudioInfo videoAudioInfo;
	if (videoFile->GetVideoAudioInfo(videoAudioInfo)) {
		if (picture.Create(videoAudioInfo.Video.imageSize)) {
			if (videoFile->Seek(_pts)) {
				if (videoFile->ReadAFrame()) {
					if (!videoFile->Load(picture)) {
						return false;
					}
				}
			}
		}
	}

	return true;
}

bool PictureFile::UpdateVideoFrame(Picture& picture, std::unique_ptr<VideoFile>& videoFile, int64_t _pts) {
	if (!videoFile->IsOpened())
		return false;

	VideoAudioInfo videoAudioInfo;
	if (videoFile->GetVideoAudioInfo(videoAudioInfo)) {
		if (picture.Create(videoAudioInfo.Video.imageSize)) {
			//int64_t pts = _pts;
			if (videoFile->ReadAFrame()) {
				if (!videoFile->Load(picture)) {
					return false;
				}
			}
		}
	}


	return true;
}


bool PictureFile::CreatePicture(Picture& picture, const WString& filename) {
	if (!FileUtils::DoesFileExist(filename)) {
		return false;
	}
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

	if (!picture.Create(nullptr, bitmap.GetWidth(), bitmap.GetHeight(), DEFAULT_BPP, ePixelFormat::PIXEL_FORMAT_ARGB, eColorSpace::COLOR_SPACE_BT_709)) {
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