#pragma once

#include <list>

class BitWritingBlock {
	static const int BLOCK_SIZE = 255;

	std::list<unsigned char*> datas;
	unsigned char* currnet;
	int pos;
	int remain;
public:
	BitWritingBlock();
	~BitWritingBlock();

	void writeBits(unsigned int src, int bit);
	void writeByte(unsigned char b);
	bool toFile(FILE* dst);
};