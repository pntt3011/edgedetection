#include "util.h"

void edgeDetector(char* imgPath) {
	BMP head;
	char* edgePath = getName(imgPath);

	if (readBitmap(imgPath, head)) {
		convert2Gray(head);
		findEdge(head);
		writeBitmap(edgePath, head);
		delete[] head.ColorTable;
		delete[] head.ImgByte;
	}
	if (edgePath) delete[] edgePath;
}

int main(int argc, char** argv) {
	if (argc == 2) {
		edgeDetector(argv[1]);
	}
	else {
		cout << "Invalid number of arguments! Syntax: <.exe> <.bmp file> \n";
	}
	return 0;
}