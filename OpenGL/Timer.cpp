#include "Timer.h"

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
	std::chrono::steady_clock::time_point time = endTime;

	while (time - previousTimePoint < std::chrono::milliseconds{ 16 }) {
		time = std::chrono::steady_clock::now();
	}
	previousTimePoint = endTime;
	endTime = std::chrono::steady_clock::now();
}

