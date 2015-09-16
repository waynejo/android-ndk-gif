#pragma once

#include <stdint.h>

class DataBlock
{
private:
	uint8_t* data;
	int32_t remain;
public:
	DataBlock(uint8_t* data, int32_t remain);
	~DataBlock(void);

	bool read(uint8_t* dst, int32_t size);
	bool read(uint16_t* dst);
};

