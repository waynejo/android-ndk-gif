#pragma once

class DataBlock
{
private:
	unsigned char* data;
	int remain;
public:
	DataBlock(unsigned char* data, int remain);
	~DataBlock(void);

	bool read(unsigned char* dst, int size);
	bool read(unsigned short* dst);
};

