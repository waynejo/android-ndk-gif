#include "GifEncoder.h"
#include <vector>

using namespace std;

GifEncoder::GifEncoder()
{
	// init width, height to 1, to prevent divide by zero.
	width = 1;
	height = 1;

	frameNum = 0;
	lastPixels = NULL;
	fp = NULL;
}

void GifEncoder::init(unsigned short width, unsigned short height, const char* fileName)
{
	this->width = width;
	this->height = height;

	if (NULL != lastPixels) {
		delete[] lastPixels;
	}
	lastPixels = new unsigned int[width * height];

	fp = fopen(fileName, "wb");
	writeHeader();
	writeContents();
}

void GifEncoder::release()
{
	if (NULL != lastPixels) {
		delete[] lastPixels;
	}

	if (NULL != fp) {
		fclose(fp);
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
	unsigned int* pixelBegin = pixels;
	while (last != pixels) {
		++colors[RED][(*pixels) & 0xFF];
		++colors[GREEN][((*pixels) >> 8) & 0xFF];
		++colors[BLUE][((*pixels) >> 16) & 0xFF];
		++pixels;
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
	mapColor(cubes, 256, pixelBegin);
}

void GifEncoder::mapColor(Cube* cubes, int cubeNum, unsigned int* pixels)
{
	int pixelNum = width * height;
	unsigned int* last = pixels + pixelNum;
	Cube* lastCube = cubes + cubeNum;
	
	while (last != pixels) {
		Cube* cube = cubes;
		unsigned int colors[COLOR_MAX];
		colors[RED] = (*pixels) & 0xFF;
		colors[GREEN] = ((*pixels) >> 8) & 0xFF;
		colors[BLUE] = ((*pixels) >> 16) & 0xFF;

		while (lastCube != cube) {
			bool inCube = true;
			for (int color = 0; color < COLOR_MAX; ++color) {
				if (cube->cMin[color] > colors[color] || cube->cMax[color] < colors[color]) {
					inCube = false;
					break;
				}
			}
			if (inCube) {
				break;
			}
			++cube;
		}
		unsigned int blue = cube->cMin[BLUE] + (cube->cMax[BLUE] - cube->cMin[BLUE]) / 2;
		unsigned int green = cube->cMin[GREEN] + (cube->cMax[GREEN] - cube->cMin[GREEN]) / 2;
		unsigned int red = cube->cMin[RED] + (cube->cMax[RED] - cube->cMin[RED]) / 2;
		(*pixels) = (blue << 16) | (green << 8) | red;
		++pixels;
	}
}

void GifEncoder::writeHeader()
{
	fwrite("GIF89a", 6, 1, fp);
	writeLSD();
}

bool GifEncoder::writeLSD()
{
	// logical screen size
	fwrite(&width, 2, 1, fp);
	fwrite(&height, 2, 1, fp);

	// packed fields
	unsigned char gctFlag = 0; // 1 : global color table flag
	unsigned char colorResolution = 8; // only 8 bit
	unsigned char oderedFlag = 0;
	unsigned char gctSize = 0;
	unsigned char packed = (gctFlag << 7) | ((colorResolution - 1) << 4) | (oderedFlag << 3) | gctSize;
	fwrite(&packed, 1, 1, fp);

	unsigned char backgroundColorIndex = 0;
	fwrite(&backgroundColorIndex, 1, 1, fp);

	unsigned char aspectRatio = 0;
	fwrite(&aspectRatio, 1, 1, fp);

	return true;
}

bool GifEncoder::writeContents()
{
	writeNetscapeExt();

	return true;
}

bool GifEncoder::writeNetscapeExt()
{
	//                                   code extCode,                                                            size,       loop count, end
	const unsigned char netscapeExt[] = {0x21, 0xFF, 0x0B, 'N', 'E', 'T', 'S', 'C', 'A', 'P', 'E', '2', '.', '0', 0x03, 0x01, 0x00, 0x00, 0x00};
	fwrite(netscapeExt, sizeof(netscapeExt), 1, fp);
	return true;
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
