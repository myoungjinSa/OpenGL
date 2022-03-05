#pragma once
#include "FileIO.h"
#include "Picture.h"

WString imageList[];
WString videoList[];

class VideoFile;
class Codec;

class PictureFile {
public:
	class CodecLimits {
		virtual bool IsSupported(const Codec& codec);
	};
	class Factory {
		Factory();
		virtual ~Factory();

		virtual VideoFile* CreateVideoFile(const WString& filename);


	};

	PictureFile();
	virtual ~PictureFile();

	virtual bool Create(Picture& picture, const WString& filename) { return true; }
	virtual bool Load(Picture& picture, const WString& filename) { return true; }
	virtual bool Save(Picture& picture, const WString& filename) { return true; }

	static bool IsVideoFile(const WString& filename);
	static bool IsImageFile(const WString& filename);

	static bool CreateVideoFrame(Picture& picture, const WString& filename, int frameNo);
	static bool CreatePicture(Picture& picture, const WString& filename);
};

class GDIPictureFile : public PictureFile{
public:
	GDIPictureFile();
	~GDIPictureFile() override;
	bool Create(Picture& picture, const WString& filename) override;
	bool Load(Picture& picture, const WString& filename) override;
	bool Save(Picture& picture, const WString& filename) override;

private:
	static bool LoadGDIPlus();
	static void UnloadGDIPlus();
	static unsigned __int64 gdiplusToken;
};

class VideoInfo {
public:
	VideoInfo();

private:
	Size2i imageSize;
	int	   bpp;
	ePixelFormat pixelFormat;
	eColorSpace colorSpace;
	__int64 startFrameNo;
	__int64 frameCount;
};

class AudioInfo {
public:
	unsigned int channelCount;
	unsigned int samplePerSecond;
	unsigned int bitsPerSample;
	
	AudioInfo() {
		Reset();
	}

	bool IsValid() const {
		return channelCount != 0 && samplePerSecond != 0 && bitsPerSample != 0;
	}
	void Setup(unsigned int _channelCount, unsigned int _samplesPerSecond, unsigned int _bitsPerSample) {
		channelCount = _channelCount;
		samplePerSecond = _samplesPerSecond;
		bitsPerSample = _bitsPerSample;
	}

	void Reset() {
		channelCount = 0;
		samplePerSecond = 0;
		bitsPerSample = 0;
	}
};


class VideoAudioInfo {
public:
	VideoInfo Video;
	AudioInfo Audio;
};