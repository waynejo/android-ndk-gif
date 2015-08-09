#pragma once

#include "DataBlock.h"
#include "GifFrame.h"
#include <vector>

class GifDecoder
{
private:
	static const int MAX_STACK_SIZE = 4096;

	std::vector<GifFrame> frames;
	unsigned short width;
	unsigned short height;
	bool interlace;
	bool gctFlag;
	unsigned int gctSize;
	unsigned char bgIndex;
	unsigned char pixelAspect;
	unsigned int gct[256]; // [0] ->r, [1] -> g, [2] -> b, max size to avoid bounds checks
	unsigned char block[256];
	unsigned short ix, iy, iw, ih;
	unsigned int bgColor;
	unsigned int loopCount; // iterations; 0 = repeat forever
	unsigned int dispose; // 0=no action; 1=leave in place; 2=restore to bg; 3=restore to prev
	bool transparency; // use transparent color
	unsigned short delay;
	unsigned char transIndex;
	int frameCount;
	unsigned char* pixels;

	unsigned int lastDispose;
	unsigned short lrx, lry, lrw, lrh;
	unsigned int lastBgColor;
	unsigned int* image;
	const unsigned int* lastBitmap;

	void init();

	bool readLSD(DataBlock* dataBlock);
	bool readColorTable(DataBlock* dataBlock, unsigned int* colorTable, int ncolors);
	bool readHeader(DataBlock* dataBlock);

	bool readContents(DataBlock* dataBlock);
	bool skip(DataBlock* dataBlock);
	bool readBlock(DataBlock* dataBlock, unsigned char* blockSize);
	bool readNetscapeExt(DataBlock* dataBlock);
	bool readGraphicControlExt(DataBlock* dataBlock);
	bool readBitmap(DataBlock* dataBlock);
	void resetFrame();
	bool decodeBitmapData(DataBlock* dataBlock);
	void setPixels(unsigned int* act);

public:
	GifDecoder(void);
	~GifDecoder(void);

	bool load(const char* fileName);
	const unsigned int* getFrame(int n);

	unsigned int getWidth();
	unsigned int getHeight();
};

