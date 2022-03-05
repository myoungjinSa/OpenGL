#pragma once
#ifndef video_reader_hpp
#define video_reader_hpp

#include "PictureFile.h"

class VideoFormat {
public:
	enum class eFrameRate {
		FRAME_RATE_2398,
		FRAME_RATE_2400,
		FRAME_RATE_2500,
		FRAME_RATE_2997,
		FRAME_RATE_3000,
		FRAME_RATE_5000,
		FRAME_RATE_5994,
		FRAME_RATE_6000,

		FRAME_RATE_COUNT,
	};

	enum eScan {
		SCAN_PROGRESSIVE,
		SCAN_UPPER_FIELD_FIRST,
		SCAN_LOWER_FIELD_FIRST,

		SCAN_COUNT,
	};
	VideoFormat();

	bool Setup(const Size2i& screenSize, const Size2i& aspect, eScan scan, float frameRate);
	bool IsValid() const;
private:
	Size2i	screenSize;
	Size2i	aspectRatio;
	eScan	scanMode;
	float	frameRate;
	int		fps;
};


class Codec {
public:
	enum class eCodec {
		CODEC_H264,

		CODEC_COUNT
	};

	enum class eFileType {
		TYPE_AVI,
		TYPE_MOV,
		TYPE_WMV,
		TYPE_MXF,
		TYPE_MP4,
		TYPE_MPG,

		TYPE_COUNT,
	};

	eCodec codec;
	eFileType fileType;
	eColorSpace colorSpace;
};


class VideoFile {
public:
	virtual ~VideoFile() {}

	virtual bool OpenFile(const WString& filename, ePixelFormat pixelFormat) = 0;
	virtual bool IsOpened() const = 0;
	virtual void Close() = 0;

	virtual bool Play() { return true; }
	virtual bool Seek() { return true; }
};

#endif