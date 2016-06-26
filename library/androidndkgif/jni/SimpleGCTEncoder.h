
#pragma once

class SimpleGCTGifEncoder : public IGifEncoder
{
	static const int32_t MAX_STACK_SIZE = 4096;
	static const int32_t BYTE_NUM = 256;
	static const int R_RANGE = 6;
	static const int G_RANGE = 7;
	static const int B_RANGE = 6;

	uint16_t width;
	uint16_t height;
	int32_t frameNum;
	bool useDither;
	uint32_t* lastPixels;
	uint32_t* lastColorReducedPixels;
	uint32_t lastRootColor;
	FILE* fp;

	void reduceColor(uint32_t* pixels);
	void removeSamePixels(uint8_t* src1, uint8_t* src2, EncodeRect* rect);

	void writeHeader();
	bool writeLSD();
	void writeGCT(FILE* fp);
	bool writeContents(uint8_t* pixels, uint16_t delay, const EncodeRect& encodingRect);
	bool writeNetscapeExt();
	bool writeGraphicControlExt(uint16_t delay);
	bool writeFrame(uint8_t* pixels, const EncodeRect& encodingRect);
	bool writeLCT(int32_t colorNum, Cube* cubes);
	bool writeBitmapData(uint8_t* pixels, const EncodeRect& encodingRect);
public:
	SimpleGCTGifEncoder();
	virtual ~SimpleGCTGifEncoder();

	virtual bool init(uint16_t width, uint16_t height, const char* fileName);
	virtual void release();
	virtual void setDither(bool useDither);
	virtual uint16_t getWidth();
	virtual uint16_t getHeight();

	virtual void encodeFrame(uint32_t* pixels, int32_t delayMs);
};