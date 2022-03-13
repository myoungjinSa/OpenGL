#include "RingBuffer.h"
#include <time.h>
#include <thread>

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

constexpr std::chrono::nanoseconds timespecToDuration(timespec ts) {
	auto duration = std::chrono::seconds(ts.tv_sec) + std::chrono::nanoseconds(ts.tv_nsec);
	return std::chrono::duration_cast<std::chrono::nanoseconds>(duration);
}

RingBuffer::RingBuffer(size_t _size, float _elementsPerSecond) 
	: size(_size)
	, elementsPerSecond(_elementsPerSecond)
	, readPoint(0)
	, writePoint(0)
	, pData(nullptr)
{
	pData = new float[size];
}

RingBuffer::~RingBuffer() {
	if (pData)
		delete[] pData;
}

bool RingBuffer::CanWrite(int elements) {
	return (writePoint + elements <= readPoint + size);
}

void RingBuffer::Write(int elements, int* size_1, float** buffer_1, int* size_2, float** buffer_2) {
	int writePointMod = writePoint % size;

	int size_1_max = size - writePointMod;

	*size_1 = MIN(size_1_max, elements);
	*buffer_1 = &pData[writePointMod];
	*size_2 = elements - *size_1;
	*buffer_2 = &pData[0];
}

void RingBuffer::WriteWait(int elements, int* size_1, float** buffer_1, int* size_2, float** buffer_2){
	while (true) {
		int available = readPoint + size - writePoint;
		if (elements <= available) {
			break;
		}

		int remaining = elements - available;
		float sleepTime = 0.25 * remaining / elementsPerSecond;
		struct timespec ts;
		ts.tv_sec = (int)sleepTime;
		ts.tv_nsec = (sleepTime - ts.tv_sec) * 1000000000;

		std::this_thread::sleep_for(timespecToDuration(ts));
	}
	Write(elements, size_1, buffer_1, size_2, buffer_2);
}

void RingBuffer::WriteAdvance(int elements) {
	writePoint += elements;
}

bool RingBuffer::CanRead(int elements) {
	return (readPoint + elements <= writePoint);
}

void RingBuffer::Read(int elements, int* size_1, float** buffer_1, int* size_2, float** buffer_2) {
	int readPointMod = readPoint % size;

	int size_1_max = size - readPointMod;
	*size_1 = MIN(size_1_max, elements);
	*buffer_1 = &pData[readPointMod];
	*size_2 = elements - *size_1;
	*buffer_2 = &pData[0];
}

void RingBuffer::ReadWait(int elements, int* size_1, float** buffer_1, int* size_2, float** buffer_2){
	while (true)
	{
		int available = writePoint - readPoint;
		if (elements <= available) {
			break;
		}

		int remaining = elements - available;
		float sleepTime = 0.25 * remaining / elementsPerSecond;
		struct timespec ts;
		ts.tv_sec = (int)sleepTime;
		ts.tv_nsec = (sleepTime - ts.tv_sec) * 1000000000;
		std::this_thread::sleep_for(timespecToDuration(ts));
	}
	Read(elements, size_1, buffer_1, size_2, buffer_2);
}

void RingBuffer::ReadAdvance(int elements) {
	readPoint += elements;
}






