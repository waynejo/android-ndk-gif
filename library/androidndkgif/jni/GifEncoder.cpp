#include "GifEncoder.h"
#include "BitWritingBlock.h"
#include <vector>
#include <stdio.h>

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

void GifEncoder::init(uint16_t width, uint16_t height, const char* fileName)
{
	this->width = width;
	this->height = height;

	if (NULL != lastPixels) {
		delete[] lastPixels;
	}
	lastPixels = new uint32_t[width * height];

	fp = fopen(fileName, "wb");
	writeHeader();
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

void GifEncoder::removeSamePixels(uint32_t* dst, uint32_t* src1, uint32_t* src2)
{
}

void GifEncoder::qsortColorHistogram(uint32_t* imageColorHistogram, int32_t maxColor, uint32_t from, uint32_t to)
{
	if (to == from) {
		return ;
	}
	uint32_t middle = from + ((to - from) >> 1);
	uint32_t shift = maxColor << 3;
	uint32_t pivot = ((imageColorHistogram[middle]) >> shift) & 0xFF;
	uint32_t i = from;
	uint32_t k = to;
	while (i <= k) {
		while (((imageColorHistogram[i] >> shift) & 0xFF) < pivot && i <= k) {
			++i;
		}
		while (((imageColorHistogram[k] >> shift) & 0xFF) > pivot && i <= k) {
			--k;
		}
		if (i <= k) {
			uint32_t temp = imageColorHistogram[k];
			imageColorHistogram[k] = imageColorHistogram[i];
			imageColorHistogram[i] = temp;
			++i;
			--k;
		}
	}
	if (from < k) {
		qsortColorHistogram(imageColorHistogram, maxColor, from, k);
	}
	if (i < to) {
		qsortColorHistogram(imageColorHistogram, maxColor, i, to);
	}
}

void GifEncoder::updateColorHistogram(Cube* nextCube, Cube* maxCube, int32_t maxColor, uint32_t* imageColorHistogram)
{
	qsortColorHistogram(imageColorHistogram, maxColor, maxCube->colorHistogramFromIndex, maxCube->colorHistogramToIndex);
	uint32_t median = maxCube->colorHistogramFromIndex + ((maxCube->colorHistogramToIndex - maxCube->colorHistogramFromIndex) >> 1);
	nextCube->colorHistogramFromIndex = maxCube->colorHistogramFromIndex;
	nextCube->colorHistogramToIndex = median;
	maxCube->colorHistogramFromIndex = median + 1;
	nextCube->cMin[maxColor] = GET_COLOR(imageColorHistogram[nextCube->colorHistogramFromIndex], maxColor);
	nextCube->cMax[maxColor] = GET_COLOR(imageColorHistogram[nextCube->colorHistogramToIndex], maxColor);
	maxCube->cMin[maxColor] = GET_COLOR(imageColorHistogram[maxCube->colorHistogramFromIndex], maxColor);
	maxCube->cMax[maxColor] = GET_COLOR(imageColorHistogram[maxCube->colorHistogramToIndex], maxColor);
}

void GifEncoder::computeColorTable(uint32_t* pixels, Cube* cubes)
{
	uint32_t colors[COLOR_MAX][Cube::COLOR_RANGE] = {0, };
	
	uint32_t pixelNum = width * height;
	uint32_t* last = pixels + pixelNum;
	uint32_t* pixelBegin = pixels;

	vector<uint32_t> colorHistogramMemory;
	colorHistogramMemory.resize(pixelNum * sizeof(uint32_t));
	uint32_t *colorHistogram = &colorHistogramMemory[0];
	memcpy(colorHistogram, pixels, pixelNum * sizeof(uint32_t));

	while (last != pixels) {
		uint8_t r = (*pixels) & 0xFF;
		uint8_t g = ((*pixels) >> 8) & 0xFF;
		uint8_t b = ((*pixels) >> 16) & 0xFF;
		++colors[RED][r];
		++colors[GREEN][g];
		++colors[BLUE][b];
		++pixels;
	}

	uint32_t cubeIndex = 0;
	Cube* cube = &cubes[cubeIndex];
	for (uint32_t i = 0; i < COLOR_MAX; ++i) {
		cube->cMin[i] = 255;
		cube->cMax[i] = 0;
	}
	for (uint32_t i = 0; i < 256; ++i) {
		for (uint32_t color = 0; color < COLOR_MAX; ++color) {
			if (0 != colors[color][i]) {
				cube->cMax[color] = cube->cMax[color] < i ? i : cube->cMax[color];
				cube->cMin[color] = cube->cMin[color] > i ? i : cube->cMin[color];
			}
		}
	}
	cube->colorHistogramFromIndex = 0;
	cube->colorHistogramToIndex = pixelNum - 1;
	for (cubeIndex = 1; cubeIndex < 255; ++cubeIndex) {
		uint32_t maxDiff = 0;
		uint32_t maxColor = GREEN;
		Cube* maxCube = cubes;
		for (uint32_t i = 0; i < cubeIndex; ++i) {
			Cube* cube = &cubes[i];
			uint32_t color = GREEN;
			uint32_t diff = cube->cMax[GREEN] - cube->cMin[GREEN];
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
		if (1 >= maxDiff) {
			break;
		}
		Cube* nextCube = &cubes[cubeIndex + 1];
		for (int32_t color = 0; color < COLOR_MAX; ++color) {
			if (color == maxColor) {
				updateColorHistogram(nextCube, maxCube, maxColor, colorHistogram);
			} else {
				nextCube->cMax[color] = maxCube->cMax[color];
				nextCube->cMin[color] = maxCube->cMin[color];
			}
		}
	}
	for (uint32_t i = 0; i < 256; ++i) {
		Cube* cube = &cubes[i];
		for (int32_t color = 0; color < COLOR_MAX; ++color) {
			cube->color[color] = cube->cMin[color] + (cube->cMax[color] - cube->cMin[color]) / 2;
		}
	}

	mapColor(cubes, 256, pixelBegin);
}

void GifEncoder::mapColor(Cube* cubes, uint32_t cubeNum, uint32_t* pixels)
{
	uint32_t pixelNum = width * height;
	uint32_t* last = pixels + pixelNum;
	uint8_t* pixelOut = (uint8_t*)pixels;
	
	while (last != pixels) {
		Cube* cube = cubes;
		uint32_t r = (*pixels) & 0xFF;
		uint32_t g = ((*pixels) >> 8) & 0xFF;
		uint32_t b = ((*pixels) >> 16) & 0xFF;

		bool isFound = false;
		for (uint32_t cubeId = 0; cubeId < cubeNum; ++cubeId) {
			Cube* cube = cubes + cubeId;
			if (cube->cMin[RED] <= r && r <= cube->cMax[RED] &&
				cube->cMin[GREEN] <= g && g <= cube->cMax[GREEN] &&
				cube->cMin[BLUE] <= b && b <= cube->cMax[BLUE]) {
				*pixelOut = cubeId;
				isFound = true;
				break;
			}
		}
		++pixels;
		++pixelOut;
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
	uint8_t gctFlag = 0; // 1 : global color table flag
	uint8_t colorResolution = 8; // only 8 bit
	uint8_t oderedFlag = 0;
	uint8_t gctSize = 0;
	uint8_t packed = (gctFlag << 7) | ((colorResolution - 1) << 4) | (oderedFlag << 3) | gctSize;
	fwrite(&packed, 1, 1, fp);

	uint8_t backgroundColorIndex = 0;
	fwrite(&backgroundColorIndex, 1, 1, fp);

	uint8_t aspectRatio = 0;
	fwrite(&aspectRatio, 1, 1, fp);

	return true;
}

bool GifEncoder::writeContents(Cube* cubes, uint8_t* pixels)
{
	writeNetscapeExt();

	writeGraphicControlExt();
	writeFrame(cubes, pixels);

	return true;
}

bool GifEncoder::writeNetscapeExt()
{
	//                                   code extCode,                                                            size,       loop count, end
	const uint8_t netscapeExt[] = {0x21, 0xFF, 0x0B, 'N', 'E', 'T', 'S', 'C', 'A', 'P', 'E', '2', '.', '0', 0x03, 0x01, 0x00, 0x00, 0x00};
	fwrite(netscapeExt, sizeof(netscapeExt), 1, fp);
	return true;
}

bool GifEncoder::writeGraphicControlExt()
{
	uint8_t disposalMethod = 1; // Do not dispose
	uint8_t userInputFlag = 0; // User input is not expected.
	uint8_t transparencyFlag = 1; // Transparent Index is given.

	uint8_t packed = (disposalMethod << 2) | (userInputFlag << 1) | transparencyFlag;
	//                                                     size, packed, delay(2), transIndex, terminator
	const uint8_t graphicControlExt[] = {0x21, 0xF9, 0x04, packed, 0x00, 0x0a, 0xFF, 0x00};
	fwrite(graphicControlExt, sizeof(graphicControlExt), 1, fp);
	return true;
}

bool GifEncoder::writeFrame(Cube* cubes, uint8_t* pixels)
{
	uint8_t code = 0x2C;
	fwrite(&code, 1, 1, fp);
	uint16_t ix = 0;
	uint16_t iy = 0;
	uint16_t iw = width;
	uint16_t ih = height;
	uint8_t localColorTableFlag = 1;
	uint8_t interlaceFlag = 0;
	uint8_t sortFlag = 0;
	uint8_t sizeOfLocalColorTable = 7;
	uint8_t packed = (localColorTableFlag << 7) | (interlaceFlag << 6) | (sortFlag << 5) | sizeOfLocalColorTable;
	fwrite(&ix, 2, 1, fp);
	fwrite(&iy, 2, 1, fp);
	fwrite(&iw, 2, 1, fp);
	fwrite(&ih, 2, 1, fp);
	fwrite(&packed, 1, 1, fp);

	writeLCT(2 << sizeOfLocalColorTable, cubes);
	writeBitmapData(pixels);
	return true;
}

bool GifEncoder::writeLCT(int32_t colorNum, Cube* cubes)
{
	uint32_t color;
	Cube* cube;
	for (int32_t i = 0; i < colorNum; ++i) {
		cube = cubes + i;
		color = cube->color[RED] | (cube->color[GREEN] << 8) | (cube->color[BLUE] << 16);
		fwrite(&color, 3, 1, fp);
	}
	return true;
}

bool GifEncoder::writeBitmapData(uint8_t* pixels)
{
	uint32_t pixelNum = width * height;
	uint8_t* endPixels = pixels + pixelNum;
	uint8_t dataSize = 8;
	uint32_t codeSize = dataSize + 1;
	uint32_t codeMask = (1 << codeSize) - 1;
	BitWritingBlock writingBlock;
	fwrite(&dataSize, 1, 1, fp);

	vector<uint16_t> lzwInfoHolder;
	lzwInfoHolder.resize(MAX_STACK_SIZE * BYTE_NUM);
	uint16_t* lzwInfos = &lzwInfoHolder[0];
	
	uint32_t clearCode = 1 << dataSize;
	writingBlock.writeBits(clearCode, codeSize);
	uint32_t infoNum = clearCode + 2;
	uint16_t current = *pixels;
	
	++pixels;
	
	uint16_t* next;
	while (endPixels != pixels) {
		next = &lzwInfos[current * BYTE_NUM + *pixels];
		if (0 == *next || *next >= MAX_STACK_SIZE) {
			writingBlock.writeBits(current, codeSize);
			if (*next >= MAX_STACK_SIZE) {
				writingBlock.writeBits(clearCode, codeSize);
				infoNum = clearCode + 2;
				codeSize = dataSize + 1;
				codeMask = (1 << codeSize) - 1;
				memset(lzwInfos, 0, MAX_STACK_SIZE * BYTE_NUM * sizeof(uint16_t));
				current = *pixels;
				++pixels;
				continue;
			}
			*next = infoNum;
			if (infoNum < MAX_STACK_SIZE) {
				++infoNum;
			}
			if (codeMask < infoNum - 1 && infoNum < MAX_STACK_SIZE) {
				++codeSize;
				codeMask = (1 << codeSize) - 1;
			}
			if (endPixels == pixels) {
				break;
			}
			current = *pixels;
		} else {
			current = *next;
		}
		++pixels;
	}
	writingBlock.writeBits(current, codeSize);
	writingBlock.toFile(fp);
	return true;
}

void GifEncoder::encodeFrame(uint32_t* pixels, int delayMs)
{
	uint32_t pixelNum = width * height;
	uint32_t* frame = new uint32_t[pixelNum];
	if (0 == frameNum) {
		memcpy(frame, pixels, pixelNum * sizeof(uint32_t));
	} else {
		removeSamePixels(frame, lastPixels, pixels);
	}
	Cube cubes[256];
	computeColorTable(pixels, cubes);
	writeContents(cubes, (uint8_t*)pixels);

	memcpy(lastPixels, pixels, pixelNum * sizeof(uint32_t));
	++frameNum;
	
	delete[] frame;
}
