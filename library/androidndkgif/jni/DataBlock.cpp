#include "DataBlock.h"
#include <string.h>

DataBlock::DataBlock(const uint8_t* data, int32_t remain) : 
	remain(remain)
{
	this->data = data;
}

DataBlock::DataBlock(const DataBlock& dataBlock)
{
	this->data = dataBlock.data;
	this->remain = dataBlock.remain;
}

DataBlock::~DataBlock(void)
{
}

bool DataBlock::read(uint8_t* dst, int32_t size)
{
	if (remain < size) {
		return false;
	}
	memcpy(dst, data, size);
	
	data += size;
	remain -= size;
	return true;
}

bool  DataBlock::read(uint16_t* dst)
{
	return read((uint8_t*)dst, 2);
}