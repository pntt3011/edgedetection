#include "util.h"

void findPaddingAndSize(BMP& header) {
	header.util.Channel = header.dib.Depth / 8;
	header.util.Padding = (4 - (header.dib.Width * header.util.Channel) % 4) % 4;
	header.util.LineSize = header.dib.Width * header.util.Channel + header.util.Padding;
	header.util.ByteSize = header.dib.Height * header.util.LineSize;
}

bool readBitmap(char* imagePath, BMP& head) {
	FILE* img = fopen(imagePath, "rb");
	if (!img) {
		cout << "Failed to load image." << endl;
		return false;
	}
	fread(&head.header, sizeof(Header), 1, img);
	fread(&head.dib, sizeof(DIB), 1, img);

	head.util.ColorTableSize = head.header.PixelArrayOffset - sizeof(Header) - sizeof(DIB);
	head.ColorTable = new unsigned char[head.util.ColorTableSize];
	fread(head.ColorTable, sizeof(char), head.util.ColorTableSize, img);

	findPaddingAndSize(head);

	head.ImgByte = new unsigned char[head.util.ByteSize];
	fread(head.ImgByte, sizeof(char), head.util.ByteSize, img);

	fclose(img);
	return true;
}

void writeBitmap(const char* edgePath, BMP& head) {
	FILE* part = fopen(edgePath, "wb");
	fwrite(&head.header, sizeof(Header), 1, part);
	fwrite(&head.dib, sizeof(DIB), 1, part);
	fwrite(head.ColorTable, sizeof(char), head.util.ColorTableSize, part);
	fwrite(head.ImgByte, sizeof(char), head.util.ByteSize, part);
	fclose(part);
}

char* getName(char* imgPath) {
	char* edgePath = new char[MAX_SIZE];
	strcpy(edgePath, imgPath);
	char* temp = edgePath + (strlen(imgPath) - 4);
	strcpy(temp, "Edge.bmp");
	*(temp + 8) = '\0';
	return edgePath;
}

void convert2Gray(BMP& head) {
	float weight[] = { 0.3, 0.59, 0.11 };
	for (int i = 0; i < head.dib.Height; i++) {
		for (int j = 0; j < head.dib.Width; j++) {
			float value = 0;
			for (int k = 0; k < head.util.Channel; k++) {
				value += head.ImgByte[i * head.util.LineSize + j * head.util.Channel + k] * weight[k];
			}
			for (int k = 0; k < head.util.Channel; k++) {
				head.ImgByte[i * head.util.LineSize + j * head.util.Channel + k] = (int)value;
			}
		}
	}
}

int sobel(BMP& head, int x, int y) {
	if (x > head.dib.Height - FILTER_SIZE || y > head.dib.Width - FILTER_SIZE) return 0;
	int sobelX[][3] = { {1,0,-1},{2,0,-2},{1,0,-1} };
	int sobelY[][3] = { {-1,-2,-1},{0,0,0},{1,2,1} };
	int valueX = 0;
	int valueY = 0;
	for (int i = 0; i < FILTER_SIZE; i++) {
		for (int j = 0; j < FILTER_SIZE; j++) {
			valueX += head.ImgByte[(x + i) * head.util.LineSize + (y + j) * head.util.Channel] * sobelX[i][j];
			valueY += head.ImgByte[(x + i) * head.util.LineSize + (y + j) * head.util.Channel] * sobelY[i][j];
		}
	}
	return (int)sqrt(valueX * valueX + valueY * valueY);
}

int findSobel(BMP& head, int* tempValue) {
	int max = 0;
	for (int i = 0; i < head.dib.Height; i++) {
		for (int j = 0; j < head.dib.Width; j++) {
			int value = sobel(head, i, j);
			max = max > value ? max : value;
			tempValue[i * head.dib.Width + j] = value;
		}
	}
	return max;
}

void findEdge(BMP& head) {
	int* tempValue = new int[head.dib.Height * head.dib.Width];
	int max = findSobel(head, tempValue);
	for (int i = 0; i < head.dib.Height; i++) {
		for (int j = 0; j < head.dib.Width; j++) {
			for (int k = 0; k < head.util.Channel; k++) {
				head.ImgByte[i * head.util.LineSize + j * head.util.Channel + k] = (unsigned char)(tempValue[i * head.dib.Width + j] * 255.0 / max);
			}
		}
	}
	if (tempValue) delete[] tempValue;
}
