#pragma once

#include "EncodingType.h"

class GifEncoder {
private:
	IGifEncoder* gifEncoder;
public:
	GifEncoder(EncodingType encodingType = EncodingType::ENCODING_TYPE_NORMAL_LOW_MEMORY);

	bool init(uint16_t width, uint16_t height, const char* fileName);
	void release();
	void setDither(bool useDither);
	uint16_t getWidth();
	uint16_t getHeight();

	void encodeFrame(uint32_t* pixels, int32_t delayMs);
};