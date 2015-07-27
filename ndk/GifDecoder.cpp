#include "GifDecoder.h"
#include "DataBlock.h"

GifDecoder::GifDecoder(void)
{
}


GifDecoder::~GifDecoder(void)
{
}

bool GifDecoder::load(const char* fileName)
{
	loopCount = 1;
	dispose = 0;
	transparency = false;
	delay = 0;
	FILE* fp = fopen(fileName, "rb");
	if (NULL == fp) {
		return false;
	}
	fseek(fp, 0, SEEK_END);
	int fileSize = ftell(fp);
	unsigned char* data = new unsigned char[fileSize];
	rewind(fp);
	fread(data, fileSize, 1, fp);
	fclose(fp);

	if (!readHeader(DataBlock(data, fileSize))) {
		return false;
	}
}

bool GifDecoder::readLSD(DataBlock* dataBlock) {
	// logical screen size
	unsigned short width;
	unsigned short height;
	if (!dataBlock->read(&width) || !dataBlock->read(&height)) {
		return false;
	}

	// packed fields
	unsigned char packed;
	if (!dataBlock->read(&packed, 1)) {
		return false;
	}
	gctFlag = (packed & 0x80) != 0; // 1 : global color table flag
	// 2-4 : color resolution
	// 5 : gct sort flag
	gctSize = 2 << (packed & 7); // 6-8 : gct size
	if (!dataBlock->read(&bgIndex, 1)) { // background color index
		return false;
	}
	if (!dataBlock->read(&pixelAspect, 1)) { // pixel aspect ratio
		return false;
	}
}

bool GifDecoder::readColorTable(DataBlock* dataBlock, unsigned char* colorTable, int ncolors) {
	int nbytes = 3 * ncolors;
	return dataBlock->read((unsigned char*)colorTable, nbytes);
}

bool GifDecoder::readHeader(DataBlock dataBlock) {
	unsigned char buffer[6];
	if (!dataBlock.read(buffer, 6)) {
		return false;
	}
	if (0 != memcmp("GIF", buffer, 3)) {
		return false;
	}

    if (!readLSD(&dataBlock)) {
		return false;
	}

	if (gctFlag) {
		if (!readColorTable(&dataBlock, (unsigned char*)gct, gctSize)) {
			return false;
		}
		bgColor = 0xFF000000 | (gct[bgIndex][0] << 16) | (gct[bgIndex][1] << 8) | gct[bgIndex][2];
 	}
}

bool GifDecoder::readContents(DataBlock* dataBlock) {
	// read GIF file content blocks
	unsigned char code;
	while (true) {
		if (!dataBlock->read(&code, 1)) {
			return false;
		}
		switch (code) {
		case 0x2C: // image separator
			if (!readBitmap(dataBlock)) {
				return false;
			}
			break;
		case 0x21: // extension
			if (!dataBlock->read(&code, 1)) {
				return false;
			}
			switch (code) {
			case 0xf9: // graphics control extension
				if (!readGraphicControlExt(dataBlock)) {
					return false;
				}
				break;
			case 0xff: // application extension
				unsigned char blockSize;
				readBlock(dataBlock, &blockSize);
				if (0 == memcmp("NETSCAPE2.0", block, 11)) {
					if (!readNetscapeExt(dataBlock)) {
						return false;
					}
				} else { // don't care
					if (!skip(dataBlock)) {
						return false;
					}
				}
				break;
			case 0xfe:// comment extension
				if (!skip(dataBlock)) {
					return false;
				}
				break;
			case 0x01:// plain text extension
				if (!skip(dataBlock)) {
					return false;
				}
				break;
			default: // uninteresting extension
				if (!skip(dataBlock)) {
					return false;
				}
			}
			break;
		case 0x3b: // terminator
			return true;
			break;
		case 0x00: // bad byte, but keep going and see what happens break;
		default:
			return false;
		}
	}
}

bool GifDecoder::skip(DataBlock* dataBlock)
{
	unsigned char blockSize;
	do {
		if (!readBlock(dataBlock, &blockSize)) {
			return false;
		}
	} while ((blockSize > 0));
	return true;
}

bool GifDecoder::readBlock(DataBlock* dataBlock, unsigned char* blockSize)
{
	dataBlock->read(blockSize, 1);
	return blockSize <= 0 || dataBlock->read(block, *blockSize);
}

bool GifDecoder::readNetscapeExt(DataBlock* dataBlock)
{
	unsigned char blockSize;
	do {
		if (!readBlock(dataBlock, &blockSize)) {
			return false;
		}
		if (block[0] == 1) {
			// loop count sub-block
			int b1 = ((int) block[1]) & 0xff;
			int b2 = ((int) block[2]) & 0xff;
			loopCount = (b2 << 8) | b1;
		}
	} while ((blockSize > 0));
	return true;
}

bool GifDecoder::readGraphicControlExt(DataBlock* dataBlock)
{
	unsigned char unused;
	if (!dataBlock->read(&unused, 1)) { // block size
		return false;
	}
	unsigned char packed;
	if (!dataBlock->read(&packed, 1)) { // packed fields)
		return false;
	}
	dispose = (packed & 0x1c) >> 2; // disposal method
	if (dispose == 0) {
		dispose = 1; // elect to keep old image if discretionary
	}
	transparency = (packed & 1) != 0;
	if (!dataBlock->read(&delay)) {
		return false;
	}
	delay *= 10; // delay in milliseconds
	if (!dataBlock->read(&transIndex, 1)) {// transparent color index)
		return false;
	}
	return dataBlock->read(&unused, 1); // block terminator
}

bool GifDecoder::readBitmap(DataBlock* dataBlock)
{
	unsigned short ix, iy, iw, ih;
	unsigned char packed;
	if (!dataBlock->read(&ix) || !dataBlock->read(&iy) ||  // (sub)image position & size
		!dataBlock->read(&iw) || !dataBlock->read(&ih) || !dataBlock->read(&packed, 1)) {
		return false;
	}

	bool lctFlag = (packed & 0x80) != 0; // 1 - local color table flag interlace
	int lctSize = 2 << ((packed & 0x07) + 1);
	// 3 - sort flag
	// 4-5 - reserved lctSize = 2 << (packed & 7); // 6-8 - local color
	// table size
	bool interlace = (packed & 0x40) != 0;
	unsigned char lct[256][3];
	unsigned char (*act)[3];
	if (lctFlag) {
		if (!readColorTable(dataBlock, (unsigned char*)lct, lctSize)) { // read table
			return false;
		}
		act = lct; // make local table active
	} else {
		act = gct; // make global table active
		if (bgIndex == transIndex) {
			bgColor = 0;
		}
	}
	char save[3];
	if (transparency) {
		memcpy(save, act[transIndex], 3);
		memset(act[transIndex], 0, 3); // set transparent color if specified
	}

	if (!decodeBitmapData(dataBlock) || !skip(dataBlock)) { // decode pixel data
		return false;
	}

	if (transparency) {
		memcpy(act[transIndex], save, 3);
	}
	resetFrame();
}

void GifDecoder::resetFrame()
{
	lastDispose = dispose;
	lrx = ix;
	lry = iy;
	lrw = iw;
	lrh = ih;
	lastBgColor = bgColor;
	dispose = 0;
	transparency = false;
	delay = 0;
}

bool GifDecoder::decodeBitmapData(DataBlock* dataBlock)
{
	int nullCode = -1;
	int npix = iw * ih;
	int available, clear, code_mask, code_size, end_of_information, in_code, old_code, bits, code, i, datum, first, top, bi, pi;
	unsigned char count, data_size;
	unsigned char* pixels = new unsigned char[npix]; // allocate new pixel array
	unsigned short prefix[MAX_STACK_SIZE];
	unsigned char suffix[MAX_STACK_SIZE];
	unsigned char pixelStack[MAX_STACK_SIZE + 1];

	// Initialize GIF data stream decoder.
	if (!dataBlock->read(&data_size, 1)) {
		return false;
	}
	clear = 1 << data_size;
	end_of_information = clear + 1;
	available = clear + 2;
	old_code = nullCode;
	code_size = data_size + 1;
	code_mask = (1 << code_size) - 1;
	for (code = 0; code < clear; code++) {
		prefix[code] = 0; // XXX ArrayIndexOutOfBoundsException
		suffix[code] = (byte) code;
	}
	// Decode GIF pixel stream.
	datum = bits = first = top = pi = bi = count = 0;
	for (i = 0; i < npix;) {
		if (top == 0) {
			if (bits < code_size) {
				// Load bytes until there are enough bits for a code.
				if (count == 0) {
					// Read a new data block.
					if (!readBlock(dataBlock, &count)) {
						delete[] pixels;
						return false;
					}
					if (count <= 0) {
						break;
					}
					bi = 0;
				}
				datum += (((int) block[bi]) & 0xff) << bits;
				bits += 8;
				bi++;
				count--;
				continue;
			}
			// Get the next code.
			code = datum & code_mask;
			datum >>= code_size;
			bits -= code_size;
			// Interpret the code
			if ((code > available) || (code == end_of_information)) {
				break;
			}
			if (code == clear) {
				// Reset decoder.
				code_size = data_size + 1;
				code_mask = (1 << code_size) - 1;
				available = clear + 2;
				old_code = nullCode;
				continue;
			}
			if (old_code == nullCode) {
				pixelStack[top++] = suffix[code];
				old_code = code;
				first = code;
				continue;
			}
			in_code = code;
			if (code == available) {
				pixelStack[top++] = (byte) first;
				code = old_code;
			}
			while (code > clear) {
				pixelStack[top++] = suffix[code];
				code = prefix[code];
			}
			first = ((int) suffix[code]) & 0xff;
			// Add a new string to the string table,
			if (available >= MAX_STACK_SIZE) {
				break;
			}
			pixelStack[top++] = (byte) first;
			prefix[available] = (short) old_code;
			suffix[available] = (byte) first;
			available++;
			if (((available & code_mask) == 0) && (available < MAX_STACK_SIZE)) {
				code_size++;
				code_mask += available;
			}
			old_code = in_code;
		}
		// Pop a pixel off the pixel stack.
		top--;
		pixels[pi++] = pixelStack[top];
		i++;
	}
	for (i = pi; i < npix; i++) {
		pixels[i] = 0; // clear missing pixels
	}
	delete[] pixels;
}