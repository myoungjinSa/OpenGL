#include "Picture.h"

Picture::Picture() 
	: pImage(nullptr)
	, width(0)
	, height(0)
	, bpp(0)
	, pitch(0)
{

}

Picture::~Picture() {
	Destroy();
}

void Picture::Destroy() {
	if (pImage)
		delete[] pImage;

	pImage = nullptr;
	width = 0;
	height = 0;
	bpp = 0;
	pitch = 0;
}

bool Picture::Create(void* pData, int _width, int _height, int _bpp) {
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
	}
	else {
		pImage = pData;
	}

	assert(pImage);
	return true;
}