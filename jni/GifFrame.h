#pragma once

class GifFrame
{
public:
	GifFrame(unsigned int* data);
	~GifFrame(void);

	const unsigned int* data;
};

