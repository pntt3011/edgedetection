#pragma once
#pragma pack(1)
#pragma warning(disable: 4996)

#define MAX_SIZE	256
#define FILTER_SIZE	3

#include <stdint.h>
#include <fstream>
#include <iostream>
#include <math.h>

using namespace std;

struct Header {
	uint16_t Signature;
	uint32_t Size;
	uint16_t Reserved1;
	uint16_t Reserved2;
	uint32_t PixelArrayOffset;
};

struct DIB {
	uint32_t DIBSize;
	uint32_t Width;
	uint32_t Height;
	uint16_t Plane;
	uint16_t Depth;
	uint32_t Compress;
	uint32_t PixelArraySize;
	uint32_t xResolution;
	uint32_t yResolution;
	uint32_t ColorTable;
	uint32_t ColorImportant;
};

struct Util {
	int Channel;
	int Padding;
	int LineSize;
	int ByteSize;
	int ColorTableSize;
};

struct BMP {
	Header header;
	DIB dib;
	unsigned char* ColorTable;
	unsigned char* ImgByte;
	Util util;
};

void findPaddingAndSize(BMP& header);
bool readBitmap(char* imagePath, BMP& head);
void writeBitmap(const char* edgePath, BMP& head);
char* getName(char* imgPath);
void convert2Gray(BMP& head);
void findEdge(BMP& head);
