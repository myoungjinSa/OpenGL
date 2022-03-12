#include "Timer.h"
#include "Logger.h"

Timer::Timer()
	: bStopped(false),
	  elapsedTime(0),
		startTime(),
		endTime(),
	    previousTimePoint()
{

}

Timer::~Timer() {

}

void Timer::Start() {
	startTime = std::chrono::steady_clock::now();
	
	endTime = startTime;
	previousTimePoint = endTime;
	bStopped = false;
}

void Timer::Stop() {
	bStopped = true;
}

void Timer::Reset() {
	startTime = std::chrono::steady_clock::now();
	endTime = std::chrono::steady_clock::now();
	previousTimePoint = std::chrono::steady_clock::now();
}

long long Timer::GetTotalTime() const {
	return std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
}
float Timer::GetEalapsedTime(){
	return (float)std::chrono::duration_cast<std::chrono::milliseconds>(endTime - previousTimePoint).count() / 1000.0;
}

void Timer::Tick(float fLockFps) {
	using Frame = std::chrono::duration<int32_t, std::ratio<1, 60>>;
	using MS = std::chrono::duration<float, std::milli>;

	endTime = std::chrono::steady_clock::now();
	previousTimePoint = endTime;
	Frame fps{};

	//while (true) {
	//	fps = std::chrono::duration_cast<Frame>(std::chrono::steady_clock::now() - endTime);
	//	if (1/*sec*/<= fps.count()) {
	//		endTime = std::chrono::steady_clock::now();
	//		//LogDebug(L"LastFrame: %.5lf, FPS: %.5lf\n", std::chrono::duration_cast<MS>(fps).count(), fps.count() * 60.0f);
	//		break;
	//	}
	//}
}

