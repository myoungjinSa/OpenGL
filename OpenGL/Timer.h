#pragma once
#include <chrono>

class Timer {
public:
	Timer();
	~Timer();

	void Tick(float fLockFPS = 0.0f);
	void Start();
	void Stop();
	void Reset();


	float GetEalapsedTime();
	long long GetTotalTime() const;
public:
	std::chrono::steady_clock::time_point startTime;
	std::chrono::steady_clock::time_point endTime;

	std::chrono::steady_clock::time_point previousTimePoint;

	bool bStopped;
	long long elapsedTime;
};