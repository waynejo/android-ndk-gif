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
	int width;
	int height;
	int frameNum;
	unsigned int* lastPixels;

	void removeSamePixels(unsigned int* dst, unsigned int* src1, unsigned int* src2);
	void computeColorTable(unsigned int* pixels);
	void mapColor(Cube* cubes, int cubeNum, unsigned int* pixels);
public:
	GifEncoder();

	void init(int width, int height);
	void release();

	void encodeFrame(unsigned int* pixels, int delayMs);
};