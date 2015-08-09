#include <string.h>
#include "DataBlock.h"


DataBlock::DataBlock(unsigned char* data, int remain) :
	remain(remain)
{
	this->data = data;
}


DataBlock::~DataBlock(void)
{
}

bool DataBlock::read(unsigned char* dst, int size)
{
	if (remain < size) {
		return false;
	}
	memcpy(dst, data, size);

	data += size;
	remain -= size;
}

bool  DataBlock::read(unsigned short* dst)
{
	return read((unsigned char*)dst, 2);
}
