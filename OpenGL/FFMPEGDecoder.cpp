#include "FFMPEGDecoder.h"

#pragma comment(lib, "avdevice.lib")
#include <assert.h>
extern "C" {
#include <libavdevice/avdevice.h>
}
#include "Logger.h"



bool FFMPGVideoReader::OpenFile(const WString& filename, ePixelFormat pixelFormat) {

	//avdevice_register_all();

	//auto& width = this->width;
	//auto& height = this->height;
	//auto& channelNum = this->channelNum;
	//auto& sampleRate = this->sampleRate;
	//auto& timeBase = this->timeBase;
	//auto& pFormatContext = this->pFormatContext;
	//auto& pPacket = this->pPacket;

	//auto& pVideoCodecContext = this->pVideoCodecContext;
	//auto& videoStreamIndex = this->videoStreamIndex;
	//auto& pVideoFrame = this->pVideoFrame;

	//auto& pAudioCodecContext = this->pAudioCodecContext;
	//auto& audioStreamIndex = this->audioStreamIndex;
	//auto& pAudioFrame = this->pAudioFrame;

	//this->pSwsScalerContext = NULL;

	//pFormatContext = avformat_alloc_context();
	//if (!pFormatContext) {
	//	LogError(L"Couldn't created AVFormatContext\n");
	//	return false;
	//}

	//const AVInputFormat* inputFormat = NULL;
	//do {
	//	inputFormat = av_input_video_device_next(inputFormat);
	//} while (inputFormat != NULL && strcmp(inputFormat->name, "dshow") != 0);


	//if (!inputFormat) {
	//	LogError(L"Couldn't find DShow input format to get webcam\n");
	//}

	//if (avformat_open_input(&pFormatContext, filename, NULL, NULL) != 0) {
	//	LogError(L"Couldn't open video file\n");
	//	return false;
	//}

	////Find the first valid video stream inside the file
	//videoStreamIndex = -1;
	//audioStreamIndex = -1;

	//AVCodecParameters* pVideoCodecParams = NULL;
	//AVCodecParameters* pAudioCodecParams = NULL;

	//const AVCodec* pVideoCodec = NULL;
	//const AVCodec* pAudioCodec = NULL;

	//for (int iStream = 0; iStream < pFormatContext->nb_streams; ++iStream) {
	//	auto codecParams = pFormatContext->streams[iStream]->codecpar;
	//	auto codec = avcodec_find_decoder(codecParams->codec_id);
	//	if (!codec) {
	//		continue;
	//	}

	//	if (videoStreamIndex == -1 && codecParams->codec_type == AVMEDIA_TYPE_VIDEO) {
	//		videoStreamIndex = iStream;
	//		pVideoCodecParams = codecParams;
	//		pVideoCodec = codec;

	//		width = codecParams->width;
	//		height = codecParams->height;

	//		timeBase = pFormatContext->streams[iStream]->time_base;
	//		continue;
	//	}

	//	if (audioStreamIndex == -1 && codecParams->codec_type == AVMEDIA_TYPE_AUDIO) {
	//		audioStreamIndex = iStream;
	//		pAudioCodecParams = codecParams;
	//		pAudioCodec = codec;
	//		
	//		channelNum = codecParams->channels;
	//		sampleRate = codecParams->sample_rate;

	//		continue;
	//	}
	//}

	//if (videoStreamIndex == -1 || audioStreamIndex == -1) {
	//	LogError(L"Couldn't find valid stream inside the file\n");
	//	return false;
	//}

	////set up a codec context for the decoder
	//pVideoCodecContext = avcodec_alloc_context3(pVideoCodec);
	//if (!pVideoCodecContext) {
	//	LogError(L"Couldn't create AVCodecContext\n");
	//	return false;
	//}

	//if (avcodec_parameters_to_context(pVideoCodecContext, pVideoCodecParams) < 0) {
	//	LogError(L"Couldn't initialize AVCodecContext\n");
	//	return false;
	//}

	//if (avcodec_open2(pVideoCodecContext, pVideoCodec, NULL) < 0) {
	//	LogError(L"Couldn't open codec\n");
	//	return false;
	//}

	//pVideoFrame = av_frame_alloc();
	//if (!pVideoFrame) {
	//	LogError(L"Couldn't allocate AVFrame\n");
	//	return false;
	//}

	////Setup a codec context for the audio decoder
	//pAudioCodecContext = avcodec_alloc_context3(pAudioCodec);
	//if (!pAudioCodecContext) {
	//	LogError(L"Couldn't create AVCodecContext\n");
	//	return false;
	//}

	//if (avcodec_parameters_to_context(pAudioCodecContext, pAudioCodecParams) < 0) {
	//	LogError(L"Couldnt' intialize AVCodecContext\n");
	//	return false;
	//}

	//if (avcodec_open2(pAudioCodecContext, pAudioCodec, NULL) < 0) {
	//	LogError(L"Coudln't open codec\n");
	//	return false;
	//}

	//pAudioFrame = av_frame_alloc();
	//if (!pAudioFrame) {
	//	LogError(L"Couldn't allocate AVFrame\n");
	//	return false;
	//}

	//pPacket = av_packet_alloc();
	//if (!pPacket) {
	//	LogError(L"Couldn't allocate AVPacket\n");
	//	return false;
	//}


	return true;
}

void FFMPGVideoReader::Close() {
	/*sws_freeContext(pSwsScalerContext);
	avformat_close_input(&pFormatContext);
	avformat_free_context(pFormatContext);
	av_frame_free(&pVideoFrame);
	av_frame_free(&pAudioFrame);

	av_packet_free(&pPacket);
	avcodec_free_context(&pVideoCodecContext);
	avcodec_free_context(&pAudioCodecContext);*/
}
