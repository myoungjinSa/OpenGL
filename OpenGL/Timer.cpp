#include "Timer.h"

Timer::Timer()
	: bStopped(false),
	  elapsedTime(0),
		startTime(),
		endTime()
{

}

Timer::~Timer() {

}

void Timer::Start() {
	startTime = std::chrono::steady_clock::now();
	
	endTime = startTime;
	bStopped = false;
}

void Timer::Stop() {
	bStopped = true;
}

void Timer::Reset() {
	startTime = std::chrono::steady_clock::now();
	endTime = std::chrono::steady_clock::now();
}

long long Timer::GetEalapsedTime(){
	return std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
}

void Timer::Tick(float fLockFps) {
	endTime = std::chrono::steady_clock::now();
}

