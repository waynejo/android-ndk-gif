#pragma once

struct EncodeRect {
	int32_t x;
	int32_t y;
	int32_t width;
	int32_t height;
};

enum COLOR {
	RED = 0,
	GREEN,
	BLUE,
	COLOR_MAX
};

struct Cube {
	static const int COLOR_RANGE = 256;

	uint32_t cMin[COLOR_MAX];
	uint32_t cMax[COLOR_MAX];
	uint32_t colorHistogramFromIndex;
	uint32_t colorHistogramToIndex;
	uint32_t color[COLOR_MAX];
};

#define GET_COLOR(color, colorIdx) (((color) >> ((colorIdx) << 3)) & 0xFF)
#define ABS(v) (0 > (v) ? -(v) : (v))
#define ABS_DIFF(a, b) ((a) > (b) ? (a) - (b) : (b) - (a))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

class IGifEncoder
{
public:
	virtual ~IGifEncoder() {}

	virtual bool init(uint16_t width, uint16_t height, const char* fileName) = 0;
	virtual void release() = 0;
	virtual void setDither(bool useDither) = 0;
	virtual uint16_t getWidth() = 0;
	virtual uint16_t getHeight() = 0;

	virtual void encodeFrame(uint32_t* pixels, int32_t delayMs) = 0;
};