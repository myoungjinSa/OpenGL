#include "AudioDevice.h"
#include "Logger.h"
#include <assert.h>


AudioDevice::AudioCallback PortAudioDevice::userCallback;
int PortAudioDevice::numChannels;

static int Pa_CustomCallback(const void* pInput, void* pOutput, unsigned long frameCount, const PaStreamCallbackTimeInfo* pTimeInfo, PaStreamCallbackFlags statusFlags, void* pUserData) {
	float* pBuffer = (float*)pOutput;

	PortAudioDevice::userCallback(PortAudioDevice::numChannels, frameCount, pBuffer);
	return paContinue;
}

PortAudioDevice::PortAudioDevice() 
	:pStream(nullptr)
{

}

PortAudioDevice::~PortAudioDevice() {
	pStream = nullptr;
}

bool PortAudioDevice::Init(int _numChannels, float sampleRate, int bufferSize, AudioDevice::AudioCallback callback) {
	numChannels = _numChannels;

	userCallback = callback;

	PaError err;
	err = Pa_Initialize();
	if (err != paNoError) {
		LogError(L"PortAudio error : %s\n", Pa_GetErrorText(err));
		return false;
	}

	err = Pa_OpenDefaultStream(&pStream, 0, numChannels, paFloat32, sampleRate, bufferSize, Pa_CustomCallback, NULL);
	if (err != paNoError) {
		LogError(L"PortAudio error : %s\n", Pa_GetErrorText(err));
		return false;
	}
	Start();
	return true;
}

bool PortAudioDevice::Release() {
	PaError err;
	err = Pa_Terminate();
	if (err != paNoError) {
		return false;
	}

	return true;
}

void PortAudioDevice::Start() {
	PaError err;
	assert(pStream);
	err = Pa_StartStream(pStream);
	if (err != paNoError) {
		LogError(L"PortAudio error : 5s\n", Pa_GetErrorText(err));
	}
}


void PortAudioDevice::Stop() {
	PaError err;
	assert(pStream);
	err = Pa_StopStream(pStream);

	if (err != paNoError) {
		LogError(L"PortAudio error : %s\n", Pa_GetErrorText(err));
	}
}

