#include "GifEncoder.h"
#include <vector>

using namespace std;

enum COLOR {
	RED,
	GREEN,
	BLUE
};

struct Cube {
	int min;
	int max;
	COLOR color;
};

GifEncoder::GifEncoder()
{
	// init width, height to 1, to prevent divide by zero.
	width = 1;
	height = 1;

	frameNum = 0;
	lastPixels = NULL;
}

void GifEncoder::init(int width, int height)
{
	this->width = width;
	this->height = height;

	if (NULL != lastPixels) {
		delete[] lastPixels;
	}
	lastPixels = new unsigned int[width * height];
}

void GifEncoder::release()
{
	if (NULL != lastPixels) {
		delete[] lastPixels;
	}
}

void GifEncoder::removeSamePixels(unsigned int* dst, unsigned int* src1, unsigned int* src2)
{
}

void GifEncoder::computeColorTable(unsigned int* pixels)
{
	int r[256], g[256], b[256];
	int pixelNum = width * height;
	unsigned int* last = pixels + pixelNum;
	while (last != pixels) {
		++r[(*pixels) & 0xFF];
		++g[((*pixels) >> 8) & 0xFF];
		++b[((*pixels) >> 16) & 0xFF];
	}

	Cube cubes[256];
	int cubeIndex = 0;
	Cube* cube = &cubes[cubeIndex];
	cube->min = 255;
	cube->max = 0;
	for (int i = 0; i < 256; ++i) {
		if (0 == r[i]) {
			continue;
		}
		cube->max = cube->max < i ? i : cube->max;
		cube->min = cube->min > i ? i : cube->min;
	}
}

void GifEncoder::encodeFrame(unsigned int* pixels, int delayMs)
{
	int pixelNum = width * height;
	unsigned int* frame = new unsigned int[pixelNum];
	if (0 == frameNum) {
		memcpy(frame, pixels, pixelNum * sizeof(unsigned int));
	} else {
		removeSamePixels(frame, lastPixels, pixels);
	}
	computeColorTable(pixels);

	memcpy(lastPixels, pixels, pixelNum * sizeof(unsigned int));
	++frameNum;

	delete[] frame;
}