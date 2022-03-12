#pragma once
#include <functional>
#include <portAudio/portaudio.h>

#pragma comment(lib, "portaudio.lib")

class AudioDevice {
public:
	typedef std::function<void(int, int, float*)> AudioCallback;

	virtual bool Init(int _numChannels, float sampleRate, int bufferSize, AudioDevice::AudioCallback callback) = 0;
	virtual bool Release() = 0;

	virtual void Start() = 0;
	virtual void Stop() = 0;
};


class PortAudioDevice : public AudioDevice{
public:
	static AudioDevice::AudioCallback userCallback;
	static int numChannels;
	
	PortAudioDevice();
	virtual ~PortAudioDevice();

	bool Init(int _numChannels, float sampleRate, int bufferSize, AudioDevice::AudioCallback callback) override;
	bool Release() override;

	void Start() override;
	void Stop() override;
private:
	PaStream* pStream;
};
