#include "FFMPEGDecoder.h"
#include "PictureFile.h"
#include "AudioDevice.h"

#pragma warning(disable : 4996)
#pragma comment(lib, "avdevice.lib")

#include <assert.h>
extern "C" {
#include <libavdevice/avdevice.h>
}
#include "Logger.h"

constexpr int BUFFER_SIZE = 512;

static const char* av_make_error(int errnum) {
	static char str[AV_ERROR_MAX_STRING_SIZE];
	memset(str, 0, sizeof(str));
	return av_make_error_string(str, AV_ERROR_MAX_STRING_SIZE, errnum);
}

FFMPGVideoReader::FFMPGVideoReader()
	: width(0)
	, height(0)
	, channelNum(0)
	, sampleRate(0)
	, timeBase()
	, pixelFormat()
	, sampleFormat(AV_SAMPLE_FMT_NONE)
	, pFormatContext(nullptr)
	, pPacket(nullptr)
	, videoStreamIndex(-1)
	, pVideoCodecContext(nullptr)
	, pVideoFrame(nullptr)
	, pSwsScalerContext(nullptr)
	, audioStreamIndex(-1)
	, pAudioCodecContext(nullptr)
	, pAudioFrame(nullptr)
	, pAudioDevice(nullptr)
	, pRingbuffer(nullptr)
{
	avdevice_register_all();
}

FFMPGVideoReader::~FFMPGVideoReader() {
	ReleaseAudioDevice();
}

bool FFMPGVideoReader::IsOpened() const {
	return IsVideoValid();
}

bool FFMPGVideoReader::IsValid() const {
	return IsVideoValid() && IsAudioValid();
}
bool FFMPGVideoReader::IsVideoValid() const {
	return pFormatContext && videoStreamIndex != -1 && pVideoFrame && pVideoCodecContext && pPacket;
}

bool FFMPGVideoReader::IsAudioValid() const {
	return pFormatContext && audioStreamIndex != -1 && pAudioFrame && pAudioCodecContext && pPacket;
}

long long FFMPGVideoReader::CalcPts(float milliSec) const {
	return milliSec * timeBase.den / timeBase.num;
}

bool FFMPGVideoReader::OpenFile(const WString& filename, ePixelFormat pixelFormat) {
	pFormatContext = avformat_alloc_context();
	if (!pFormatContext) {
		LogError(L"Couldn't created AVFormatContext\n");
		return false;
	}

	const AVInputFormat* inputFormat = NULL;
	do {
		inputFormat = av_input_video_device_next(inputFormat);
	} while (inputFormat != NULL && strcmp(inputFormat->name, "dshow") != 0);


	if (!inputFormat) {
		LogError(L"Couldn't find DShow input format to get webcam\n");
	}
	
	wchar_t* pWstr = const_cast<wchar_t*>(filename.c_str());
	if (!pWstr) {
		assert(0);
		return false;
	}

	size_t nLen = wcslen(pWstr);
	char* pStr = (char*)malloc(sizeof(wchar_t) * nLen + 1);
	wcstombs(pStr, pWstr, sizeof(wchar_t) * nLen);
	
	if (avformat_open_input(&pFormatContext, pStr, NULL, NULL) != 0) {
		free(pStr);
		LogError(L"Couldn't open video file\n");
		return false;
	}

	free(pStr);
	
	////Find the first valid video stream inside the file
	
	AVCodecParameters* pVideoCodecParams = NULL;
	AVCodecParameters* pAudioCodecParams = NULL;

	const AVCodec* pVideoCodec = NULL;
	const AVCodec* pAudioCodec = NULL;

	for (int iStream = 0; iStream < pFormatContext->nb_streams; ++iStream) {
		auto codecParams = pFormatContext->streams[iStream]->codecpar;
		auto codec = avcodec_find_decoder(codecParams->codec_id);
		if (!codec) {
			continue;
		}

		if (videoStreamIndex == -1 && codecParams->codec_type == AVMEDIA_TYPE_VIDEO) {
			videoStreamIndex = iStream;
			pVideoCodecParams = codecParams;
			pVideoCodec = codec;

			width = codecParams->width;
			height = codecParams->height;

			timeBase = pFormatContext->streams[iStream]->time_base;
			continue;
		}

		if (audioStreamIndex == -1 && codecParams->codec_type == AVMEDIA_TYPE_AUDIO) {
			audioStreamIndex = iStream;
			pAudioCodecParams = codecParams;
			pAudioCodec = codec;
			
			channelNum = codecParams->channels;
			sampleRate = codecParams->sample_rate;

			continue;
		}
	}

	if (videoStreamIndex == -1 || audioStreamIndex == -1) {
		LogError(L"Couldn't find valid stream inside the file\n");
		return false;
	}

	//set up a codec context for the decoder
	pVideoCodecContext = avcodec_alloc_context3(pVideoCodec);
	if (!pVideoCodecContext) {
		LogError(L"Couldn't create AVCodecContext\n");
		return false;
	}

	if (avcodec_parameters_to_context(pVideoCodecContext, pVideoCodecParams) < 0) {
		LogError(L"Couldn't initialize AVCodecContext\n");
		return false;
	}

	if (avcodec_open2(pVideoCodecContext, pVideoCodec, NULL) < 0) {
		LogError(L"Couldn't open codec\n");
		return false;
	}

	pVideoFrame = av_frame_alloc();
	if (!pVideoFrame) {
		LogError(L"Couldn't allocate AVFrame\n");
		return false;
	}

	//Setup a codec context for the audio decoder
	pAudioCodecContext = avcodec_alloc_context3(pAudioCodec);
	if (!pAudioCodecContext) {
		LogError(L"Couldn't create AVCodecContext\n");
		return false;
	}

	if (avcodec_parameters_to_context(pAudioCodecContext, pAudioCodecParams) < 0) {
		LogError(L"Couldnt' intialize AVCodecContext\n");
		return false;
	}

	if (avcodec_open2(pAudioCodecContext, pAudioCodec, NULL) < 0) {
		LogError(L"Coudln't open codec\n");
		return false;
	}

	pAudioFrame = av_frame_alloc();
	if (!pAudioFrame) {
		LogError(L"Couldn't allocate AVFrame\n");
		return false;
	}

	pPacket = av_packet_alloc();
	if (!pPacket) {
		LogError(L"Couldn't allocate AVPacket\n");
		return false;
	}


	return true;
}

void FFMPGVideoReader::Close() {
	sws_freeContext(pSwsScalerContext);
	avformat_close_input(&pFormatContext);
	avformat_free_context(pFormatContext);
	av_frame_free(&pVideoFrame);
	av_frame_free(&pAudioFrame);

	av_packet_free(&pPacket);
	avcodec_free_context(&pVideoCodecContext);
	avcodec_free_context(&pAudioCodecContext);
}

bool FFMPGVideoReader::InitAudioDevice() {
	if (!IsOpened() || !IsAudioValid())
		return false;

	if (pAudioDevice)
		delete pAudioDevice;

	pAudioDevice = new PortAudioDevice;
	if (!pAudioDevice) {
		LogError(L"Can't Create Audio Device\n");
		return false;
	}

	pRingbuffer = new RingBuffer(8192, sampleRate * channelNum);
	if (!pRingbuffer) {
		LogError(L"Can't Create RingBuffer\n");
		return false;
	}

	return pAudioDevice->Init(channelNum, sampleRate, BUFFER_SIZE, std::bind(&FFMPGVideoReader::AudioCallBack, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

void FFMPGVideoReader::ReleaseAudioDevice() {
	if (pAudioDevice) {
		pAudioDevice->Stop();
		pAudioDevice->Release();
		delete pAudioDevice;
	}
	if (pRingbuffer)
		delete pRingbuffer;
}

bool FFMPGVideoReader::GetVideoAudioInfo(VideoAudioInfo& videoAudioInfo, Codec* pCodec) {
	if (!pFormatContext)
		return false;

	videoAudioInfo.Video.imageSize.Set(width, height);
	videoAudioInfo.Video.pixelFormat = ePixelFormat::PIXEL_FORMAT_ARGB;
	videoAudioInfo.Video.colorSpace = eColorSpace::COLOR_SPACE_BT_709;
	videoAudioInfo.Video.startFrameNo = 0;
	videoAudioInfo.Video.frameCount = pFormatContext->streams[videoStreamIndex]->nb_frames;

	return true;
}


bool FFMPGVideoReader::Seek(long long frameNo) {
	if (!IsVideoValid())
		return false;
	av_seek_frame(pFormatContext, videoStreamIndex, frameNo, AVSEEK_FLAG_BACKWARD);
	avcodec_flush_buffers(pVideoCodecContext);

	return true;
}
bool FFMPGVideoReader::ReadAFrame() {
	if (!IsVideoValid())
		return false;

	int response = 0; 
	while (0 <= av_read_frame(pFormatContext, pPacket)) {
		if (pPacket->stream_index == videoStreamIndex) {
			//Video Stream
			response = avcodec_send_packet(pVideoCodecContext, pPacket);
			if (response < 0) {
				LogDebug(L"Failed to decode packet:%s\n", av_make_error(response));
				return false;
			}

			response = avcodec_receive_frame(pVideoCodecContext, pVideoFrame);
			if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
				av_packet_unref(pPacket);
				continue;
			}else if (response < 0) {
				LogDebug(L"Failed to decode packet:%s \n", av_make_error(response));
				return false;
			}

			pixelFormat = (AVPixelFormat)pVideoFrame->format;
			//Done to construct one frame
			av_packet_unref(pPacket);
			return true;
		}else if (pPacket->stream_index == audioStreamIndex) {
			//Audio Stream, Send it to video decoder and then pull frame from decoder 
			response = avcodec_send_packet(pAudioCodecContext, pPacket);
			if (response < 0) {
				LogDebug(L"Failed to decode packet: %s\n", av_make_error(response));
				return false;
			}
			response = avcodec_receive_frame(pAudioCodecContext, pAudioFrame);
			if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
				av_packet_unref(pPacket);
				continue;
			}
			else if (response < 0) {
				LogDebug(L"Failed to decode packet: %s\n", av_make_error(response));
				return false;
			}

			sampleFormat = (AVSampleFormat)pAudioFrame->format;

			//Done to construct one frame
			av_packet_unref(pPacket);
			pAudioFrame->nb_samples;

			if (pRingbuffer) {
				int size_1 = 0, size_2 = 0;
				float *pBuffer1 = nullptr, *pBuffer2 = nullptr;

				int samples = pAudioFrame->nb_samples;
				pRingbuffer->WriteWait(samples * channelNum, &size_1, &pBuffer1, &size_2, &pBuffer2);
				ReadAudioFrame(size_1, pBuffer1, size_2, pBuffer2);
				pRingbuffer->WriteAdvance(samples * channelNum);
			}

			continue;
		}

		else {
			//Packet originates from a diffrent stream, so ignore it
			av_packet_unref(pPacket);
		}
	}

	return false;
}



bool FFMPGVideoReader::Load(Picture& picture) {
	if (!IsVideoValid())
		return false;

	if (!pSwsScalerContext) {
		pSwsScalerContext = sws_getContext(width, height, pVideoCodecContext->pix_fmt, width, height, AV_PIX_FMT_RGB32, SWS_BILINEAR, NULL, NULL, NULL);
	}
	if (!pSwsScalerContext) {
		LogError(L"Couldn't initialize sw scaler\n");
		assert(0);
		return false;
	}

	unsigned char* dest[4] = { (unsigned char*)picture.GetMemory(), NULL, NULL, NULL };
	int dest_lineSize[4] = { picture.GetPitch(), 0, 0, 0 };
	sws_scale(pSwsScalerContext, pVideoFrame->data, pVideoFrame->linesize, 0, pVideoFrame->height, dest, dest_lineSize);
	return true;
} 

void FFMPGVideoReader::ReadAudioFrame(int size1, float* pBuffer1, int size2, float* pBuffer2) {
	ReadAudioFrameInternal(size1, pBuffer1, 0);
	if (0 < size2) {
		ReadAudioFrameInternal(size2, pBuffer2, size1);
	}
}

void FFMPGVideoReader::ReadAudioFrameInternal(int size, float* pBuffer, int offset) {
	if (!IsValid())
		return;

	if (sampleFormat == AV_SAMPLE_FMT_FLTP) {
		assert(channelNum == 2);
		assert(size <= pAudioFrame->nb_samples * channelNum);

		float* ptrLeftIn = (float*)pAudioFrame->data[0];
		float* ptrRightIn = (float*)pAudioFrame->data[1];

		float* ptrOut = pBuffer;
		float* ptrOutEnd = pBuffer + size;
		while (ptrOut < ptrOutEnd) {
			*ptrOut++ = *ptrLeftIn++;
			*ptrOut++ = *ptrRightIn++;
		}
	}else {
		LogDebug(L"Sample format not supported\n");
		assert(0);
	}
}

void FFMPGVideoReader::AudioCallBack(int numChannels, int bufferSize, float* pBuffer) {
	//clear the entire output buffer
	{
		float* ptr = pBuffer;
		float* ptr_end = pBuffer + bufferSize * numChannels;

		while (ptr < ptr_end) {
			*ptr++ = 0.0;
		}
	}

	if (pRingbuffer) {
		if (pRingbuffer->CanRead(numChannels * bufferSize)) {
			int size_1 = 0, size_2 = 0;
			float* pBuffer1 = nullptr, * pBuffer2 = nullptr;
			pRingbuffer->Read(numChannels * bufferSize, &size_1, &pBuffer1, &size_2, &pBuffer2);
			memcpy(pBuffer, pBuffer1, size_1 * sizeof(float));
			if (0 < size_2) {
				memcpy(pBuffer + size_1, pBuffer2, size_2 * sizeof(float));
			}
			pRingbuffer->ReadAdvance(numChannels * bufferSize);
		}
	}
}