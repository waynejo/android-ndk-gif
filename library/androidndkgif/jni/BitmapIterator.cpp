#include "BitmapIterator.h"

BitmapIterator::BitmapIterator(GifDecoder* gifDecoder, std::shared_ptr<uint8_t> data, DataBlock dataBlock) : 
	gifDecoder(gifDecoder),
	data(data),
	dataBlock(dataBlock),
	hasNextFrame(false),
	isFinished(false)
{
}

bool BitmapIterator::hasNext()
{
	if (isFinished) {
		return false;
	}
	if (hasNextFrame) {
		return true;
	}
	uint32_t lastFrameCount = gifDecoder->getFrameCount();
	bool result = gifDecoder->readContents(&dataBlock, true);
	if (result && lastFrameCount != gifDecoder->getFrameCount()) {
		hasNextFrame = true;
		return true;
	}
	isFinished = true;
	return false;
}


bool BitmapIterator::next(const uint32_t** frame, uint32_t* delayMs)
{
	if (!hasNextFrame) {
		return false;
	}
	uint32_t index = gifDecoder->getFrameCount() - 1;
	*frame = gifDecoder->getFrame(index);
	*delayMs = gifDecoder->getDelay(index);
	hasNextFrame = false;
	return true;
}