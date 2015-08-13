#pragma once

class GifFrame
{
public:
	GifFrame(unsigned int* data, int delayMs);
	~GifFrame(void);

	const unsigned int* data;
	const int delayMs;
};

