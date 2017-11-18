
#include <iostream> 
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core.hpp"
#include <stdio.h>
#include "opencv2/opencv.hpp"
#include <windows.h> 
#include <cmath>
const int NOTHRESHOLDING = 700;

using namespace cv;
using namespace std;

bool edgeDetectBW(Mat &input, Mat &dest, int thresholdLow, int kernelSize) {

	//setting up variables
	if (kernelSize % 2 != 1) { return false; }
	int increasedDimensions = kernelSize - 1;
	int borderSize = increasedDimensions / 2;
	int distCenter = borderSize + 1;
	int imgRows = input.rows;
	int imgCols = input.cols;
	int thresLow = thresholdLow;
	bool doThresholding = (thresLow != NOTHRESHOLDING);
	int matrixSize = kernelSize * kernelSize;
	int * kernel = new int[matrixSize];

	int startAt = borderSize;

	int * row1;
	int * row2;

	//copy input into larger image
	Mat imgH(imgRows + increasedDimensions, imgCols + increasedDimensions, CV_8UC1);

	uchar * p;
	uchar * pl;

	for (int x = 0; x < imgRows; x++) {
		p = input.ptr<uchar>(x);
		pl = imgH.ptr<uchar>(x + borderSize);
		for (int y = 0; y < imgCols; y++) {
			pl[y + borderSize] = p[y];
		}
	}


	//generate horizontal kernel
	for (int i = 0; i < distCenter; i++) {
		row1 = (kernel + (i * kernelSize));
		row2 = (kernel + ((increasedDimensions - i) * kernelSize));
		for (int j = 0; j < distCenter; j++) {
			if (j == borderSize) {
				*(row1 + j) = 0;
				*(row2 + j) = 0;
				continue;
			}
			*(row1 + j) = startAt - j;
			*(row1 + increasedDimensions - j) = -(startAt - j);
			*(row2 + j) = startAt - j;
			*(row2 + increasedDimensions - j) = -(startAt - j);
		}
		startAt++;
	}

	float normalization = 0;
	//find the nomalization
	for (int t = 0; t < matrixSize; t++) {
		int val = *(kernel + t);
		if (val < 0) {
			val *= -1;
		}
		normalization += val;
	}
	normalization = normalization / 8;
	cout << "NORMALIZATION: " << normalization <<  endl;



	float total = 0;
	int x, y, i, j;

	uchar * row = imgH.ptr<uchar>(0);
	float val = 0;
	int currentRow = -1;
	//convolution
	for (x = 0; x < imgRows; x++) {
		p = dest.ptr<uchar>(x);
		for (y = 0; y < imgCols; y++) {
			currentRow = -1;
			val = 0;
			for (i = 0; i < matrixSize; i++) {
				if ((i % kernelSize) == 0) {
					currentRow++;
					row = imgH.ptr<uchar>(x + currentRow);
				}
				val += row[y + (i%kernelSize)] * kernel[i];
			}
			val = val / normalization;
			if (val < 0) {
				val *= -1;
			}
			if (val > 255) {
				val = 255;
			}
			p[y] = val;
		}
	}
	//generate vertical kernel
	startAt = borderSize;
	for (int i = 0; i < distCenter; i++) {
		row1 = (kernel + (i * kernelSize));
		row2 = (kernel + ((increasedDimensions - i) * kernelSize));
		for (int j = 0; j < distCenter; j++) {
			if (i == borderSize ) {
				*(row1 + j) = 0;
				*(row1 + increasedDimensions - j) = 0;
				continue;
			}
			*(row1 + j) = startAt + j;
			*(row1 + increasedDimensions - j) = (startAt + j);
			*(row2 + j) = - (startAt + j);
			*(row2 + increasedDimensions - j) = -(startAt + j);
		}
		startAt--;
	}
	//convolution
	for (x = 0; x < imgRows; x++) {
		p = dest.ptr<uchar>(x);
		for (y = 0; y < imgCols; y++) {
			currentRow = -1;
			val = 0;
			for (i = 0; i < matrixSize; i++) {
				if ((i % kernelSize) == 0) {
					currentRow++;
					row = imgH.ptr<uchar>(x + currentRow);
				}
				val += row[y + (i%kernelSize)] * kernel[i];
			}
			val = val / normalization;
			if (val < 0) {
				val *= -1;
			}
			if (val > 255) {
				val = 255;
			}
		
			val = sqrt(p[y] * p[y] + val * val);
			if (doThresholding) {
				if (val < thresLow) {
					val = 0;
				}
				else {
					val = 255;
				}
			}else {
				if (val > 255) {
					val = 255;
				}
			}
			p[y] = val;
		}
	}
	delete[]kernel;
	return true;
}

int main() {
	Mat img = imread("circle.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	edgeDetectBW(img, img, NOTHRESHOLDING , 3);
	imshow("edge detected", img);
	waitKey(0);
	return 0;
}
