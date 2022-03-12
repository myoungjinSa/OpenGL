#pragma once
#include "VideoFile.h"
#include "RingBuffer.h"


#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "swresample.lib")
#pragma comment(lib, "swscale.lib")
#pragma comment(lib, "avutil.lib")

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
	//#include <libswresample/swresample.h>
#include <inttypes.h>
#include <libavutil/avutil.h>
}

constexpr int RECEVIED_VIDEO = -1;
constexpr int RECEVIED_NONE = 0;

class VideoAudioInfo;
class AudioDevice;

class FFMPGVideoReader : public VideoFile {
public:
	FFMPGVideoReader();
	~FFMPGVideoReader() override;

	bool OpenFile(const WString& filename, ePixelFormat pixelFormat) override;
	bool IsOpened()const;
	void Close() override;

	bool InitAudioDevice() override;
	void ReleaseAudioDevice() override;

	bool GetVideoAudioInfo(VideoAudioInfo& videoAudioInfo, Codec* pCodecInfo = nullptr) override;
	
	bool ReadAFrame() override;
	void ReadAudioFrame(int size1, float* pBuffer1, int size2, float* pBuffer2);
	bool Seek(long long frameNo) override;
	bool Load(Picture& picture) override;

	bool IsValid() const;
	bool IsVideoValid() const;
	bool IsAudioValid() const;

	long long CalcPts(float millisec) const;
	
private:
	int width;
	int height;
	int channelNum;
	int sampleRate;

	AVRational timeBase;
	AVPixelFormat pixelFormat;
	AVSampleFormat sampleFormat;

	AVFormatContext* pFormatContext;
	AVPacket* pPacket;

	//Video State
	int videoStreamIndex;
	AVCodecContext* pVideoCodecContext;
	AVFrame* pVideoFrame;
	SwsContext* pSwsScalerContext;

	//Audio State
	int audioStreamIndex;
	AVCodecContext* pAudioCodecContext;
	AVFrame* pAudioFrame;

	AudioDevice* pAudioDevice;
	RingBuffer* pRingbuffer;

	void AudioCallBack(int numChannels, int bufferSize, float* pBuffer);
	void ReadAudioFrameInternal(int size, float* pBuffer, int offset);
};