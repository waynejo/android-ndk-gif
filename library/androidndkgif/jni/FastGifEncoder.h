#pragma once

#include <pthread.h>

struct WorkerThreadData
{
	pthread_t* workerThread;
	int32_t threadNum;
	mutable pthread_mutex_t threadLock;
	mutable pthread_cond_t threadCondition;
	volatile bool shutdown;
	volatile bool requestProcess;
	volatile bool isProcessing;

	pthread_mutex_t* encoderLock;
	pthread_cond_t* encoderCondition;

	volatile uint32_t threadCount;
	volatile bool useDither;
	volatile uint16_t width;
	volatile uint16_t height;
	volatile Cube* cubes;
	volatile int32_t cubeNum;
	volatile uint32_t* pixels;
	volatile uint32_t* lastColorReducedPixels;
	volatile uint8_t* palettizedPixels;
};

class FastGifEncoder : public BaseGifEncoder
{
	static const int32_t MAX_STACK_SIZE = 4096;
	static const int32_t BYTE_NUM = 256;
	static const int32_t MAX_THREADS = 8;

	int32_t threadCount;
	int32_t nextThreadCount;

	int32_t frameNum;
	Cube* globalCubes;
	uint8_t* palettizedPixels;

	WorkerThreadData* workerThreadData;
	WorkerThreadData primaryThreadData;

	mutable pthread_mutex_t threadLock;
	mutable pthread_cond_t threadCondition;

	void removeSamePixels(uint8_t* src1, uint8_t* src2, EncodeRect* rect);

	void writeHeader();
	bool writeLSD();
	bool writeContents(Cube* cubes, uint8_t* pixels, uint16_t delay, const EncodeRect& encodingRect);
	bool writeNetscapeExt();
	bool writeGraphicControlExt(uint16_t delay);
	bool writeFrame(Cube* cubes, uint8_t* pixels, const EncodeRect& encodingRect);
	bool writeLCT(int32_t colorNum, Cube* cubes);
	bool writeBitmapData(uint8_t* pixels, const EncodeRect& encodingRect);
	void fastReduceColor(Cube* cubes, uint32_t cubeNum, uint32_t* pixels);
public:
	FastGifEncoder();
	virtual ~FastGifEncoder();

	virtual bool init(uint16_t width, uint16_t height, const char* fileName);
	virtual void release();
	virtual void setDither(bool useDither);
	virtual uint16_t getWidth();
	virtual uint16_t getHeight();
	virtual void setThreadCount(int32_t threadCount);

	virtual void encodeFrame(uint32_t* pixels, int32_t delayMs);
};