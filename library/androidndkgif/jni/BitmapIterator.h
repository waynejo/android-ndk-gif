#pragma once

#include <stdint.h>
#include "GifDecoder.h"

class BitmapIterator {
private:
	GifDecoder* gifDecoder;
	std::shared_ptr<uint8_t> data;
	DataBlock dataBlock;
	bool hasNextFrame;
	bool isFinished;
public:
	BitmapIterator(GifDecoder* gifDecoder, std::shared_ptr<uint8_t> data, DataBlock dataBlock);
	bool hasNext();
	bool next(const uint32_t** frame, uint32_t* delayMs);
};