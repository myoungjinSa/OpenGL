#include "Picture.h"

Picture::Picture() 
	: pImage(nullptr)
	, width(0)
	, height(0)
	, bpp(0)
	, pitch(0)
	, storage(eStorage::STORAGE_NONE)
{

}

Picture::~Picture() {
	Destroy();
}
Picture::Picture(const Picture& other) {

}

Picture& Picture::operator=(const Picture& other) {
	Destroy();

	width = other.width;
	height = other.height;
	bpp = other.bpp;
	pitch = other.pitch;
	if (other.pImage) {
		pImage = new unsigned char[pitch * height];
		storage = eStorage::STOARGE_ALLOCATED;

		memcpy_s(pImage, sizeof(unsigned char) * pitch * height, other.pImage, other.pitch * other.height);
	}

	return *this;
}

void Picture::Destroy() {
	if(storage == eStorage::STOARGE_ALLOCATED && pImage)
		delete[] pImage;

	pImage = nullptr;
	width = 0;
	height = 0;
	bpp = 0;
	pitch = 0;
	storage = eStorage::STORAGE_NONE;
}

bool Picture::Create(const Size2i& imageSize) {
	return Create(imageSize, ePixelFormat::PIXEL_FORMAT_ARGB, eColorSpace::COLOR_SPACE_BT_709);
}

bool Picture::Create(const Size2i& imageSize, ePixelFormat pixelFormat, eColorSpace colorSpace) {
	return Create(imageSize.width, imageSize.height, pixelFormat, colorSpace);
}

bool Picture::Create(int width, int height, ePixelFormat pixelFormat, eColorSpace colorSpace) {
	return Create(nullptr, width, height, DEFAULT_BPP, pixelFormat, colorSpace);
}

bool Picture::Create(void* pData, int _width, int _height, int _bpp, ePixelFormat pixelFormat, eColorSpace _colorSpace) {
	Destroy();

	width = _width;
	height = _height;
	bpp = _bpp;
	pitch = PictureFormat::CalcPitch(_width, _bpp);
	
	const int LIMIT_SIZE = 400 * 1024 * 1024;
	if (!pData) {
		const int requestSize = pitch * height;
		if (requestSize < 0 || LIMIT_SIZE <= requestSize) {
			return false;
		}

		pImage = new unsigned char[pitch * height];
		storage = eStorage::STOARGE_ALLOCATED;
	}
	else {
		pImage = pData;
		storage = eStorage::STORAGE_BOUND;
	}

	assert(pImage);
	return true;
}
