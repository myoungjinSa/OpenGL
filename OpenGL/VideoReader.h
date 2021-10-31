#pragma once
#ifndef video_reader_hpp
#define video_reader_hpp


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

class VideoReader {
public:
	VideoReader();
	~VideoReader();

	bool OpenFile(const char* filePath);
	void Close();


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
};

#endif