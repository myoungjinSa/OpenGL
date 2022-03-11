#pragma once
#include "FileIO.h"
#include "Picture.h"
#include "VideoFile.h"

WString imageList[];
WString videoList[];

class Codec;

class CodecLimits {
public:
	virtual bool IsSupported(const Codec& codec) {}
};

class PictureFile {
public:	
	class Factory {
	public:
		Factory() {}
		virtual ~Factory() {}

		virtual std::unique_ptr<VideoFile> CreateVideoFile(const WString& filename, const Codec& codec) { return nullptr; }
		virtual std::unique_ptr<VideoFile> CreateVideoFile(const WString& filename) { return nullptr; }

		void  SetCodecLimits(std::shared_ptr<CodecLimits>& _codecLimits) { codecLimits = _codecLimits; }
		const std::shared_ptr<CodecLimits> GetCodecLimits()const { return codecLimits; }
	protected:
		std::shared_ptr<CodecLimits> codecLimits;
	};

	PictureFile();
	virtual ~PictureFile();

	virtual bool Create(Picture& picture, const WString& filename) { return true; }
	virtual bool Load(Picture& picture, const WString& filename) { return true; }
	virtual bool Save(Picture& picture, const WString& filename) { return true; }

	static bool IsVideoFile(const WString& filename);
	static bool IsImageFile(const WString& filename);

	static bool CreatePicture(Picture& picture, const WString& filename);
	static bool CreateVideoFrame(Picture& picture, std::unique_ptr<VideoFile>& videoFile, const WString& filename, int64_t frameNo);
	static bool UpdateVideoFrame(Picture& picture, std::unique_ptr<VideoFile>& videoFile, int64_t frameNo);
	static bool SeekVideoFrame(Picture& picture, std::unique_ptr<VideoFile>& videoFile, int64_t frameNo);

	static bool AddFactory(std::unique_ptr<Factory> factory);
	static void RemoveFactory(Factory& factory);
	static void ClearFactory();

	static void SetCodecLimits(std::shared_ptr<CodecLimits>& codecLimits);
	static bool IsSupportedCodec(const Codec& codec);

protected:
	class DefaultFactory : public Factory {
	public:
		std::unique_ptr<VideoFile> CreateVideoFile(const WString& filename) override;
	};

	class FactoryPool : public Factory {
		friend class PictureFile;
	public:
		FactoryPool();
		~FactoryPool() override;

		bool Add(std::unique_ptr<Factory> factory);
		void Remove(Factory& factory);
		void Clear();
		size_t GetCount() const;

		bool IsSupported(const Codec& codec)const;
		std::unique_ptr<VideoFile> CreateVideoFile(const WString& filename) override;
		std::unique_ptr<VideoFile> CreateVideoFile(const WString& filename, const Codec& codec) override;
	protected:
		static DefaultFactory defaultFactory;
		std::vector<std::unique_ptr<Factory>> factoryCandidates;
		CodecLimits* pCodecLimits;
	};
	static FactoryPool factoryPool;
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