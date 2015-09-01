#include "GifEncoder.h"
#include <vector>

using namespace std;

enum COLOR {
	RED = 0,
	GREEN,
	BLUE,
	COLOR_MAX
};

struct Cube {
	int cMin[COLOR_MAX];
	int cMax[COLOR_MAX];
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
	int colors[COLOR_MAX][256];
	int pixelNum = width * height;
	unsigned int* last = pixels + pixelNum;
	while (last != pixels) {
		++colors[RED][(*pixels) & 0xFF];
		++colors[GREEN][((*pixels) >> 8) & 0xFF];
		++colors[BLUE][((*pixels) >> 16) & 0xFF];
	}

	Cube cubes[256];
	int cubeIndex = 0;
	Cube* cube = &cubes[cubeIndex];
	for (int i = 0; i < COLOR_MAX; ++i) {
		cube->cMin[i] = 255;
		cube->cMax[i] = 0;
	}
	for (int i = 0; i < 256; ++i) {
		for (int color = 0; color < COLOR_MAX; ++color) {
			if (0 != colors[color][i]) {
				cube->cMax[color] = cube->cMax[color] < i ? i : cube->cMax[color];
				cube->cMin[color] = cube->cMin[color] > i ? i : cube->cMin[color];
			}
		}
	}
	for (; cubeIndex < 255; ++cubeIndex) {
		int maxDiff = 0;
		int maxColor = GREEN;
		Cube* maxCube = cubes;
		for (int i = 0; i < cubeIndex; ++i) {
			Cube* cube = &cubes[i];
			int color = GREEN;
			int diff = cube->cMax[GREEN] - cube->cMin[GREEN];
			if (cube->cMax[RED] - cube->cMin[RED] > diff) {
				diff = cube->cMax[RED] - cube->cMin[RED];
				color = RED;
			}
			if (cube->cMax[BLUE] - cube->cMin[BLUE] > diff) {
				diff = cube->cMax[BLUE] - cube->cMin[BLUE];
				color = BLUE;
			}
			if (maxDiff < diff) {
				maxDiff = diff;
				maxCube = cube;
				maxColor = color;
			}
		}
		Cube* nextCube = &cubes[cubeIndex + 1];
		for (int color = 0; color < COLOR_MAX; ++color) {
			if (color == maxColor) {
				nextCube->cMin[color] = maxCube->cMin[color] + (maxCube->cMax[color] - maxCube->cMin[color] + 1) / 2;
				nextCube->cMax[color] = maxCube->cMax[color];
			} else {
				nextCube->cMin[color] = maxCube->cMin[color];
				nextCube->cMax[color] = maxCube->cMax[color];
			}
		}
		maxCube->cMax[maxColor] = nextCube->cMin[maxColor] - 1;
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