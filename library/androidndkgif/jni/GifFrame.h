#pragma once

#include <stdint.h>

class GifFrame
{
public:
	GifFrame(uint32_t* data, int32_t delayMs);
	~GifFrame(void);

	const uint32_t* data;
	const int32_t delayMs;
};

