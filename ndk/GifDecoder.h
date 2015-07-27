#pragma once

#include "DataBlock.h"

class GifDecoder
{
private:
	static const int MAX_STACK_SIZE = 4096;

	bool gctFlag;
	unsigned int gctSize;
	unsigned char bgIndex;
	unsigned char pixelAspect;
	unsigned char gct[256][3]; // [0] ->r, [1] -> g, [2] -> b, max size to avoid bounds checks
	unsigned char block[256];
	unsigned short ix, iy, iw, ih;
	unsigned int bgColor;
	unsigned int loopCount; // iterations; 0 = repeat forever
	unsigned int dispose; // 0=no action; 1=leave in place; 2=restore to bg; 3=restore to prev
	bool transparency; // use transparent color
	unsigned short delay;
	unsigned char transIndex;


	unsigned int lastDispose;
	unsigned short lrx, lry, lrw, lrh;
	unsigned int lastBgColor;

	bool readLSD(DataBlock* dataBlock);
	bool readColorTable(DataBlock* dataBlock, unsigned char* colorTable, int ncolors);
	bool readHeader(DataBlock dataBlock);

	bool readContents(DataBlock* dataBlock);
	bool skip(DataBlock* dataBlock);
	bool readBlock(DataBlock* dataBlock, unsigned char* blockSize);
	bool readNetscapeExt(DataBlock* dataBlock);
	bool readGraphicControlExt(DataBlock* dataBlock);
	bool readBitmap(DataBlock* dataBlock);
	void resetFrame();
	bool decodeBitmapData(DataBlock* dataBlock);
public:
	GifDecoder(void);
	~GifDecoder(void);

	bool load(const char* fileName);
};

