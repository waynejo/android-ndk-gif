#pragma once

#include <list>
#include <stdint.h>
#include <stdio.h>

class BitWritingBlock {
	static const int32_t BLOCK_SIZE = 255;

	std::list<uint8_t*> datas;
	uint8_t* currnet;
	int pos;
	int remain;
public:
	BitWritingBlock();
	~BitWritingBlock();

	void writeBits(uint32_t src, int32_t bit);
	void writeByte(uint8_t b);
	bool toFile(FILE* dst);
};
