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

bool GifEncoder::init(uint16_t width, uint16_t height, const char* fileName)
{
	this->width = width;
	this->height = height;

	fp = fopen(fileName, "wb");
	if (NULL == fp) {
		return false;
	}
	if (NULL != lastPixels) {
		delete[] lastPixels;
	}
	lastPixels = new uint32_t[width * height];

	writeHeader();
	return true;
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

uint16_t GifEncoder::getWidth()
{
	return width;
}

uint16_t GifEncoder::getHeight()
{
	return height;
}

void GifEncoder::removeSamePixels(uint8_t* src1, uint8_t* src2, EncodeRect* rect)
{
	int32_t bytesPerLine = width * 4;
	int32_t beginY = 0;
	for (; beginY < height - 1; ++beginY) {
		if (0 != memcmp(src1 + bytesPerLine * beginY, src2 + bytesPerLine * beginY, bytesPerLine)) {
			break;
		}
	}
	int32_t endY = height - 1;
	for (; beginY + 1 <= endY; --endY) {
		if (0 != memcmp(src1 + bytesPerLine * endY, src2 + bytesPerLine * endY, bytesPerLine)) {
			break;
		}
	}
	++endY;

	int32_t lastY = width * height;
	bool isSame = true;
	int32_t beginX = 0;
	for (; beginX < width - 1 && isSame; ++beginX) {
		isSame = true;
		for (int32_t y = 0; y < lastY; y += width) {
			if (((uint32_t*)src1)[y + beginX] != ((uint32_t*)src2)[y + beginX]) {
				isSame = false;
				break;
			}
		}
	}
	--beginX;
	isSame = true;
	int32_t endX = width - 1;
	for (; beginX + 1 <= endX && isSame; --endX) {
		isSame = true;
		for (int32_t y = 0; y < lastY; y += width) {
			if (((uint32_t*)src1)[y + endX] != ((uint32_t*)src2)[y + endX]) {
				isSame = false;
				break;
			}
		}
	}
	++endX;

	rect->x = beginX;
	rect->y = beginY;
	rect->width = endX - beginX;
	rect->height = endY - beginY;
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
	maxCube->colorHistogramFromIndex = maxCube->colorHistogramToIndex > median + 1 ? median + 1 : maxCube->colorHistogramToIndex;
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
		Cube* nextCube = &cubes[cubeIndex];
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

	dither(cubes, 255, pixelBegin);
}

void GifEncoder::dither(Cube* cubes, uint32_t cubeNum, uint32_t* pixels)
{
	const int32_t ERROR_PROPAGATION_DIRECTION_NUM = 4;
	const int32_t ERROR_PROPAGATION_DIRECTION_X[] = {1, -1, 0, 1};
	const int32_t ERROR_PROPAGATION_DIRECTION_Y[] = {0, 1, 1, 1};
	const int32_t ERROR_PROPAGATION_DIRECTION_WEIGHT[] = {7, 3, 5, 1};

	uint32_t pixelNum = width * height;
	uint32_t* last = pixels + pixelNum;
	uint8_t* pixelOut = (uint8_t*)pixels;
	for (uint32_t y = 0; y < height; ++y) {
		for (uint32_t x = 0; x < width; ++x) {
			if (0 == (*pixels >> 24)) {
				*pixelOut = 255; // transparent color
			} else {
				Cube* cube = cubes;
				uint32_t r = (*pixels) & 0xFF;
				uint32_t g = ((*pixels) >> 8) & 0xFF;
				uint32_t b = ((*pixels) >> 16) & 0xFF;

				uint32_t closestColor = 0;
				uint32_t cubeR = cube->color[RED];
				uint32_t cubeG = cube->color[GREEN];
				uint32_t cubeB = cube->color[BLUE];
				uint32_t closestDifference = ABS_DIFF(cubeR, r) + ABS_DIFF(cubeG, g) + ABS_DIFF(cubeB, b);
				for (uint32_t cubeId = 1; cubeId < cubeNum; ++cubeId) {
					cube = cubes + cubeId;
					cubeR = cube->color[RED];
					cubeG = cube->color[GREEN];
					cubeB = cube->color[BLUE];
					uint32_t difference = ABS_DIFF(cubeR, r) + ABS_DIFF(cubeG, g) + ABS_DIFF(cubeB, b);

					if (difference < closestDifference) {
						closestDifference = difference;
						closestColor = cubeId;
					}
				}

				*pixelOut = closestColor;
				cube = cubes + closestColor;
				int32_t diffR = r - (uint32_t)cube->color[RED];
				int32_t diffG = g - (uint32_t)cube->color[GREEN];
				int32_t diffB = b - (uint32_t)cube->color[BLUE];
				for (int directionId = 0; directionId < ERROR_PROPAGATION_DIRECTION_NUM; ++directionId) {
					uint32_t* pixel = pixels + ERROR_PROPAGATION_DIRECTION_X[directionId] + ERROR_PROPAGATION_DIRECTION_Y[directionId] * width;
					if (x + ERROR_PROPAGATION_DIRECTION_X[directionId] >= width ||
						y + ERROR_PROPAGATION_DIRECTION_Y[directionId] >= height || 0 == (*pixels >> 24)) {
						continue;
					}
					int32_t weight = ERROR_PROPAGATION_DIRECTION_WEIGHT[directionId];
					int32_t dstR = ((int32_t)((*pixel) & 0xFF) + (diffR * weight + 8) / 16);
					int32_t dstG = (((int32_t)((*pixel) >> 8) & 0xFF) + (diffG * weight + 8) / 16);
					int32_t dstB = (((int32_t)((*pixel) >> 16) & 0xFF) + (diffB * weight + 8) / 16);
					int32_t dstA = (int32_t)(*pixel >> 24);
					int32_t newR = MIN(255, MAX(0, dstR));
					int32_t newG = MIN(255, MAX(0, dstG));
					int32_t newB = MIN(255, MAX(0, dstB));
					*pixel = (dstA << 24) | (newB << 16) | (newG << 8) | newR;
				}
			}
			++pixels;
			++pixelOut;
		}
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

bool GifEncoder::writeContents(Cube* cubes, uint8_t* pixels, uint16_t delay, const EncodeRect& encodingRect)
{
	writeNetscapeExt();

	writeGraphicControlExt(delay);
	writeFrame(cubes, pixels, encodingRect);

	return true;
}

bool GifEncoder::writeNetscapeExt()
{
	//                                   code extCode,                                                            size,       loop count, end
	const uint8_t netscapeExt[] = {0x21, 0xFF, 0x0B, 'N', 'E', 'T', 'S', 'C', 'A', 'P', 'E', '2', '.', '0', 0x03, 0x01, 0x00, 0x00, 0x00};
	fwrite(netscapeExt, sizeof(netscapeExt), 1, fp);
	return true;
}

bool GifEncoder::writeGraphicControlExt(uint16_t delay)
{
	uint8_t disposalMethod = 0; // dispose
	uint8_t userInputFlag = 0; // User input is not expected.
	uint8_t transparencyFlag = 1; // Transparent Index is given.

	uint8_t packed = (disposalMethod << 2) | (userInputFlag << 1) | transparencyFlag;
	//                                                     size, packed, delay(2), transIndex, terminator
	const uint8_t graphicControlExt[] = {0x21, 0xF9, 0x04, packed, delay & 0xFF, (delay >> 8), 0xFF, 0x00};
	fwrite(graphicControlExt, sizeof(graphicControlExt), 1, fp);
	return true;
}

bool GifEncoder::writeFrame(Cube* cubes, uint8_t* pixels, const EncodeRect& encodingRect)
{
	uint8_t code = 0x2C;
	fwrite(&code, 1, 1, fp);
	uint16_t ix = encodingRect.x;
	uint16_t iy = encodingRect.y;
	uint16_t iw = encodingRect.width;
	uint16_t ih = encodingRect.height;
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
	writeBitmapData(pixels, encodingRect);
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

bool GifEncoder::writeBitmapData(uint8_t* pixels, const EncodeRect& encodingRect)
{
	uint32_t pixelNum = width * height;
	uint8_t* endPixels = pixels + (encodingRect.y + encodingRect.height) * width + encodingRect.x + encodingRect.width;
	uint8_t dataSize = 8;
	uint32_t codeSize = dataSize + 1;
	uint32_t codeMask = (1 << codeSize) - 1;
	BitWritingBlock writingBlock;
	fwrite(&dataSize, 1, 1, fp);

	vector<uint16_t> lzwInfoHolder;
	lzwInfoHolder.resize(MAX_STACK_SIZE * BYTE_NUM);
	uint16_t* lzwInfos = &lzwInfoHolder[0];
	
	pixels = pixels + width * encodingRect.y + encodingRect.x;
	uint8_t* rowStart = pixels;
	uint32_t clearCode = 1 << dataSize;
	writingBlock.writeBits(clearCode, codeSize);
	uint32_t infoNum = clearCode + 2;
	uint16_t current = *pixels;
	
	++pixels;
	if (encodingRect.width <= pixels - rowStart) {
		rowStart = rowStart + width;
		pixels = rowStart;
	}
	
	uint16_t* next;
	while (endPixels > pixels) {
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
				if (encodingRect.width <= pixels - rowStart) {
					rowStart = rowStart + width;
					pixels = rowStart;
				}
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
			if (endPixels <= pixels) {
				break;
			}
			current = *pixels;
		} else {
			current = *next;
		}
		++pixels;
		if (encodingRect.width <= pixels - rowStart) {
			rowStart = rowStart + width;
			pixels = rowStart;
		}
	}
	writingBlock.writeBits(current, codeSize);
	writingBlock.toFile(fp);
	return true;
}

void GifEncoder::encodeFrame(uint32_t* pixels, int delayMs)
{
	uint32_t pixelNum = width * height;
	EncodeRect imageRect;
	imageRect.x = 0;
	imageRect.y = 0;
	imageRect.width = width;
	imageRect.height = height;

	if (0 != frameNum) {
		removeSamePixels((uint8_t*)lastPixels, (uint8_t*)pixels, &imageRect);
	}
	memcpy(lastPixels, pixels, pixelNum * sizeof(uint32_t));

	Cube cubes[256];
	computeColorTable(pixels, cubes);
	writeContents(cubes, (uint8_t*)pixels, delayMs / 10, imageRect);
	
	++frameNum;
}
