#include "BitWritingBlock.h"

using namespace std;

BitWritingBlock::BitWritingBlock()
{
	currnet = new unsigned char[BLOCK_SIZE];
	datas.push_back(currnet);
	pos = 0;
	remain = 8;
}

BitWritingBlock::~BitWritingBlock()
{
	for (list<unsigned char*>::iterator i = datas.begin(); i != datas.end(); ++i) {
		delete[] (*i);
	}
}

void BitWritingBlock::writeBits(unsigned int src, int bitNum)
{
	while (0 < bitNum) {
		if (remain <= bitNum) {
			currnet[pos] = (currnet[pos] << remain) | (((1 << remain) - 1) & src);
			src >>= remain;
			bitNum -= remain;
			remain = 8;
			++pos;
			if (pos == BLOCK_SIZE) {
				currnet = new unsigned char[BLOCK_SIZE];
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

void BitWritingBlock::writeByte(unsigned char b)
{
	writeBits(b, 8);
}

bool BitWritingBlock::toFile(FILE* dst)
{
	unsigned char size;
	for (list<unsigned char*>::iterator i = datas.begin(); i != datas.end(); ++i) {
		unsigned char* block = (*i);
		size = block == currnet ? (remain == 0 ? pos : pos + 1) : BLOCK_SIZE;
		fwrite(&size, 1, 1, dst);
		fwrite(block, size, 1, dst);
	}
	size = 0;
	fwrite(&size, 1, 1, dst);
	return true;
}