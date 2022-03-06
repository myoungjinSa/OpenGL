#include "VideoFile.h"
#include "PictureFile.h"
#include "Logger.h"
#include "String/String.h"



VideoFormat::VideoFormat() 
	: screenSize(0, 0)
	, aspectRatio(0, 0)
	, frameRate(0.0f)
	, fps(30)
	, scanMode(eScan::SCAN_PROGRESSIVE)
{

}

bool VideoFormat::IsValid() const {
	return 0 < screenSize.width && 0 < screenSize.height
		&& 0 < aspectRatio.width && 0 < aspectRatio.height
		&& 0 < fps
		&& 0.0f < frameRate
		&& scanMode != eScan::SCAN_COUNT;
}


bool VideoFormat::Setup(const Size2i& _screenSize, const Size2i& _aspect, eScan _scan, float _frameRate) {
	screenSize = _screenSize;
	aspectRatio = _aspect;
	scanMode = _scan;
	frameRate = _frameRate;
	fps = (int)(frameRate + 0.5f);
	return IsValid();
}


Codec::Codec() 
	: codec(eCodec::CODEC_COUNT)
	, fileType(eFileType::TYPE_COUNT)
	, colorSpace(eColorSpace::COLOR_SPACE_DEFAULT)
{
}

