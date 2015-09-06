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
};

class GifEncoder {
private:
	unsigned short width;
	unsigned short height;
	int frameNum;
	unsigned int* lastPixels;
	FILE* fp;

	void removeSamePixels(unsigned int* dst, unsigned int* src1, unsigned int* src2);
	void computeColorTable(unsigned int* pixels);
	void mapColor(Cube* cubes, int cubeNum, unsigned int* pixels);

	void writeHeader();
	bool writeLSD();
	bool writeContents();
	bool writeNetscapeExt();
	bool writeGraphicControlExt();
	bool writeFrame();
public:
	GifEncoder();

	void init(unsigned short width, unsigned short height, const char* fileName);
	void release();

	void encodeFrame(unsigned int* pixels, int delayMs);
};