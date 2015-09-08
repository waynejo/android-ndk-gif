#pragma once

enum COLOR {
	RED = 0,
	GREEN,
	BLUE,
	COLOR_MAX
};

struct Cube {
	unsigned int cMin[COLOR_MAX];
	unsigned int cMax[COLOR_MAX];
	unsigned int color[COLOR_MAX];
};

class GifEncoder {
private:
	static const int MAX_STACK_SIZE = 4096;
	static const int BYTE_NUM = 256;

	unsigned short width;
	unsigned short height;
	int frameNum;
	unsigned int* lastPixels;
	FILE* fp;

	void removeSamePixels(unsigned int* dst, unsigned int* src1, unsigned int* src2);
	void computeColorTable(unsigned int* pixels, Cube* cubes);
	void mapColor(Cube* cubes, int cubeNum, unsigned int* pixels);

	void writeHeader();
	bool writeLSD();
	bool writeContents(Cube* cubes, unsigned char* pixels);
	bool writeNetscapeExt();
	bool writeGraphicControlExt();
	bool writeFrame(Cube* cubes, unsigned char* pixels);
	bool writeLCT(int colorNum, Cube* cubes);
	bool writeBitmapData(unsigned char* pixels);
public:
	GifEncoder();

	void init(unsigned short width, unsigned short height, const char* fileName);
	void release();

	void encodeFrame(unsigned int* pixels, int delayMs);
};