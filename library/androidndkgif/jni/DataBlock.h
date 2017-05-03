#pragma once

#include <stdint.h>

class DataBlock
{
private:
	const uint8_t* data;
	int32_t remain;
public:
	DataBlock(const uint8_t* data, int32_t remain);
	DataBlock(const DataBlock& dataBlock);
	~DataBlock(void);

	bool read(uint8_t* dst, int32_t size);
	bool read(uint16_t* dst);
};

