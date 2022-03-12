#pragma once
#include <atomic>

class RingBuffer {
public:
	RingBuffer(size_t size, float _elementsPerSecond);
	~RingBuffer();

	bool CanWrite(int elements);
	void Write(int elements, int* size_1, float** buffer_1, int* size_2, float** buffer_2);
	void WriteAdvance(int elements);
	void WriteWait(int elements, int* size_1, float** buffer_1, int* size_2, float** buffer_2);

	bool CanRead(int elements);
	void Read(int elements, int* size_1, float** buffer_1, int* size_2, float** buffer_2);
	void ReadAdvance(int elements);
	void ReadWait(int elements, int* size_1, float** buffer_1, int* size_2, float** buffer_2);
private:
	size_t size;
	float elementsPerSecond;
	std::atomic_int readPoint;
	std::atomic_int writePoint;
	float* pData;
};

