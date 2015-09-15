#include "GifEncoder.h"
#include "BitWritingBlock.h"
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

void GifEncoder::qsortColorHistogram(unsigned int* imageColorHistogram, int maxColor, unsigned int from, unsigned int to)
{
	if (to == from) {
		return ;
	}
	unsigned int middle = from + ((to - from) >> 1);
	unsigned int shift = maxColor << 3;
	unsigned int pivot = ((imageColorHistogram[middle]) >> shift) & 0xFF;
	unsigned int i = from;
	unsigned int k = to;
	while (i <= k) {
		while (((imageColorHistogram[i] >> shift) & 0xFF) < pivot && i <= k) {
			++i;
		}
		while (((imageColorHistogram[k] >> shift) & 0xFF) > pivot && i <= k) {
			--k;
		}
		if (i <= k) {
			unsigned int temp = imageColorHistogram[k];
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

void GifEncoder::updateColorHistogram(Cube* nextCube, Cube* maxCube, int maxColor, unsigned int* imageColorHistogram)
{
	qsortColorHistogram(imageColorHistogram, maxColor, maxCube->colorHistogramFromIndex, maxCube->colorHistogramToIndex);
	unsigned int median = maxCube->colorHistogramFromIndex + ((maxCube->colorHistogramToIndex - maxCube->colorHistogramFromIndex) >> 1);
	nextCube->colorHistogramFromIndex = maxCube->colorHistogramFromIndex;
	nextCube->colorHistogramToIndex = median;
	maxCube->colorHistogramFromIndex = median + 1;
	nextCube->cMin[maxColor] = GET_COLOR(imageColorHistogram[nextCube->colorHistogramFromIndex], maxColor);
	nextCube->cMax[maxColor] = GET_COLOR(imageColorHistogram[nextCube->colorHistogramToIndex], maxColor);
	maxCube->cMin[maxColor] = GET_COLOR(imageColorHistogram[maxCube->colorHistogramFromIndex], maxColor);
	maxCube->cMax[maxColor] = GET_COLOR(imageColorHistogram[maxCube->colorHistogramToIndex], maxColor);
}

void GifEncoder::computeColorTable(unsigned int* pixels, Cube* cubes)
{
	int colors[COLOR_MAX][Cube::COLOR_RANGE] = {0, };
	
	int pixelNum = width * height;
	unsigned int* last = pixels + pixelNum;
	unsigned int* pixelBegin = pixels;

	vector<unsigned int> colorHistogramMemory;
	colorHistogramMemory.resize(pixelNum * sizeof(unsigned int));
	unsigned int *colorHistogram = &colorHistogramMemory[0];
	memcpy(colorHistogram, pixels, pixelNum * sizeof(unsigned int));

	while (last != pixels) {
		unsigned char r = (*pixels) & 0xFF;
		unsigned char g = ((*pixels) >> 8) & 0xFF;
		unsigned char b = ((*pixels) >> 16) & 0xFF;
		++colors[RED][r];
		++colors[GREEN][g];
		++colors[BLUE][b];
		++pixels;
	}

	int cubeIndex = 0;
	Cube* cube = &cubes[cubeIndex];
	for (int i = 0; i < COLOR_MAX; ++i) {
		cube->cMin[i] = 255;
		cube->cMax[i] = 0;
	}
	for (unsigned int i = 0; i < 256; ++i) {
		for (int color = 0; color < COLOR_MAX; ++color) {
			if (0 != colors[color][i]) {
				cube->cMax[color] = cube->cMax[color] < i ? i : cube->cMax[color];
				cube->cMin[color] = cube->cMin[color] > i ? i : cube->cMin[color];
			}
		}
	}
	cube->colorHistogramFromIndex = 0;
	cube->colorHistogramToIndex = pixelNum - 1;
	for (cubeIndex = 1; cubeIndex < 255; ++cubeIndex) {
		unsigned int maxDiff = 0;
		int maxColor = GREEN;
		Cube* maxCube = cubes;
		for (int i = 0; i < cubeIndex; ++i) {
			Cube* cube = &cubes[i];
			int color = GREEN;
			unsigned int diff = cube->cMax[GREEN] - cube->cMin[GREEN];
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
		for (int color = 0; color < COLOR_MAX; ++color) {
			if (color == maxColor) {
				updateColorHistogram(nextCube, maxCube, maxColor, colorHistogram);
			} else {
				nextCube->cMax[color] = maxCube->cMax[color];
				nextCube->cMin[color] = maxCube->cMin[color];
			}
		}
	}
	for (unsigned int i = 0; i < 256; ++i) {
		Cube* cube = &cubes[i];
		for (int color = 0; color < COLOR_MAX; ++color) {
			cube->color[color] = cube->cMin[color] + (cube->cMax[color] - cube->cMin[color]) / 2;
		}
	}

	mapColor(cubes, 256, pixelBegin);
}

void GifEncoder::mapColor(Cube* cubes, unsigned int cubeNum, unsigned int* pixels)
{
	int pixelNum = width * height;
	unsigned int* last = pixels + pixelNum;
	unsigned char* pixelOut = (unsigned char*)pixels;
	
	while (last != pixels) {
		Cube* cube = cubes;
		unsigned int r = (*pixels) & 0xFF;
		unsigned int g = ((*pixels) >> 8) & 0xFF;
		unsigned int b = ((*pixels) >> 16) & 0xFF;

		for (unsigned int cubeId = 0; cubeId < cubeNum; ++cubeId) {
			Cube* cube = cubes + cubeId;
			if (cube->cMin[RED] <= r && r <= cube->cMax[RED] &&
				cube->cMin[GREEN] <= g && g <= cube->cMax[GREEN] &&
				cube->cMin[BLUE] <= b && b <= cube->cMax[BLUE]) {
				*pixelOut = cubeId;
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

bool GifEncoder::writeContents(Cube* cubes, unsigned char* pixels)
{
	writeNetscapeExt();

	writeGraphicControlExt();
	writeFrame(cubes, pixels);

	return true;
}

bool GifEncoder::writeNetscapeExt()
{
	//                                   code extCode,                                                            size,       loop count, end
	const unsigned char netscapeExt[] = {0x21, 0xFF, 0x0B, 'N', 'E', 'T', 'S', 'C', 'A', 'P', 'E', '2', '.', '0', 0x03, 0x01, 0x00, 0x00, 0x00};
	fwrite(netscapeExt, sizeof(netscapeExt), 1, fp);
	return true;
}

bool GifEncoder::writeGraphicControlExt()
{
	unsigned char disposalMethod = 1; // Do not dispose
	unsigned char userInputFlag = 0; // User input is not expected.
	unsigned char transparencyFlag = 1; // Transparent Index is given.

	unsigned char packed = (disposalMethod << 2) | (userInputFlag << 1) | transparencyFlag;
	//                                                     size, packed, delay(2), transIndex, terminator
	const unsigned char graphicControlExt[] = {0x21, 0xF9, 0x04, packed, 0x00, 0x0a, 0xFF, 0x00};
	fwrite(graphicControlExt, sizeof(graphicControlExt), 1, fp);
	return true;
}

bool GifEncoder::writeFrame(Cube* cubes, unsigned char* pixels)
{
	unsigned char code = 0x2C;
	fwrite(&code, 1, 1, fp);
	unsigned short ix = 0;
	unsigned short iy = 0;
	unsigned short iw = width;
	unsigned short ih = height;
	unsigned char localColorTableFlag = 1;
	unsigned char interlaceFlag = 0;
	unsigned char sortFlag = 0;
	unsigned char sizeOfLocalColorTable = 7;
	unsigned char packed = (localColorTableFlag << 7) | (interlaceFlag << 6) | (sortFlag << 5) | sizeOfLocalColorTable;
	fwrite(&ix, 2, 1, fp);
	fwrite(&iy, 2, 1, fp);
	fwrite(&iw, 2, 1, fp);
	fwrite(&ih, 2, 1, fp);
	fwrite(&packed, 1, 1, fp);

	writeLCT(2 << sizeOfLocalColorTable, cubes);
	writeBitmapData(pixels);
	return true;
}

bool GifEncoder::writeLCT(int colorNum, Cube* cubes)
{
	unsigned int color;
	Cube* cube;
	for (int i = 0; i < colorNum; ++i) {
		cube = cubes + i;
		color = cube->color[RED] | (cube->color[GREEN] << 8) | (cube->color[BLUE] << 16);
		fwrite(&color, 3, 1, fp);
	}
	return true;
}

bool GifEncoder::writeBitmapData(unsigned char* pixels)
{
	int pixelNum = width * height;
	unsigned char* endPixels = pixels + pixelNum;
	unsigned char dataSize = 8;
	int codeSize = dataSize + 1;
	unsigned int codeMask = (1 << codeSize) - 1;
	BitWritingBlock writingBlock;
	fwrite(&dataSize, 1, 1, fp);

	vector<unsigned short> lzwInfoHolder;
	lzwInfoHolder.resize(MAX_STACK_SIZE * BYTE_NUM);
	unsigned short* lzwInfos = &lzwInfoHolder[0];
	
	int clearCode = 1 << dataSize;
	writingBlock.writeBits(clearCode, codeSize);
	unsigned int infoNum = clearCode + 2;
	unsigned short current = *pixels;
	
	++pixels;
	
	unsigned short* next;
	while (endPixels != pixels) {
		next = &lzwInfos[current * BYTE_NUM + *pixels];
		if (0 == *next || *next >= MAX_STACK_SIZE) {
			writingBlock.writeBits(current, codeSize);
			if (*next >= MAX_STACK_SIZE) {
				writingBlock.writeBits(clearCode, codeSize);
				infoNum = clearCode + 2;
				codeSize = dataSize + 1;
				codeMask = (1 << codeSize) - 1;
				memset(lzwInfos, 0, MAX_STACK_SIZE * BYTE_NUM * sizeof(unsigned short));
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

void GifEncoder::encodeFrame(unsigned int* pixels, int delayMs)
{
	int pixelNum = width * height;
	unsigned int* frame = new unsigned int[pixelNum];
	if (0 == frameNum) {
		memcpy(frame, pixels, pixelNum * sizeof(unsigned int));
	} else {
		removeSamePixels(frame, lastPixels, pixels);
	}
	Cube cubes[256];
	computeColorTable(pixels, cubes);
	writeContents(cubes, (unsigned char*)pixels);

	memcpy(lastPixels, pixels, pixelNum * sizeof(unsigned int));
	++frameNum;
	
	delete[] frame;
}
