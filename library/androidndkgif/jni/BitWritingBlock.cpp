#include "BitWritingBlock.h"
#include <memory>

using namespace std;

BitWritingBlock::BitWritingBlock()
{
	currnet = new uint8_t[BLOCK_SIZE];
	memset(currnet, 0, BLOCK_SIZE);
	datas.push_back(currnet);
	pos = 0;
	remain = 8;
}

BitWritingBlock::~BitWritingBlock()
{
	for (list<uint8_t*>::iterator i = datas.begin(); i != datas.end(); ++i) {
		delete[] (*i);
	}
}

void BitWritingBlock::writeBits(uint32_t src, int32_t bitNum)
{
	while (0 < bitNum) {
		if (remain <= bitNum) {
			currnet[pos] = currnet[pos] | (src << (8 - remain));
			src >>= remain;
			bitNum -= remain;
			remain = 8;
			++pos;
			if (pos == BLOCK_SIZE) {
				currnet = new uint8_t[BLOCK_SIZE];
				memset(currnet, 0, BLOCK_SIZE);
				datas.push_back(currnet);
				pos = 0;
			}
		} else {
			currnet[pos] = (currnet[pos] << bitNum) | (((1 << bitNum) - 1) & src);
			remain -= bitNum;
			bitNum = 0;
		}
	}
}

void BitWritingBlock::writeByte(uint8_t b)
{
	writeBits(b, 8);
}

bool BitWritingBlock::toFile(FILE* dst)
{
	uint8_t size;
	for (list<uint8_t*>::iterator i = datas.begin(); i != datas.end(); ++i) {
		uint8_t* block = (*i);
		size = block == currnet ? (remain == 0 ? pos : pos + 1) : BLOCK_SIZE;
		fwrite(&size, 1, 1, dst);
		fwrite(block, size, 1, dst);
	}
	return true;
}
