#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "parameter.h"
#include "image.h"
#include <vector>
#include <string>

int main() {

	parameter param;
	image inputImg;
	bool found;
	std::vector<cv::Point2f> corner_pts;
	image image;
	cv::String add = "bin/img/imgChess012.png";
	cv::Mat img = cv::imread(add);

	cv::Mat gray;
	cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
	cv::imwrite("grayImg.png", gray);
	/* 기존의 findChessboardCorners
	found = cv::findChessboardCorners(gray, cv::Size(CHECKERBOARD[0], CHECKERBOARD[1]), corner_pts);


	if (found) {
		cv::drawChessboardCorners(src, cv::Size(CHECKERBOARD[0], CHECKERBOARD[1]), corner_pts, found);		// Chessboard Corner 그리기
		cv::imshow("img", src);
		cv::waitKey();
	}
	else {
		std::cout << "fail" << std::endl;

	}*/

	//image.checkCorner2(gray);
	//image.checkCorner(gray);
	cv::Mat Harris = image.checkCorner3(img);
	cv::imshow("Harris", Harris);
	cv::waitKey();

	/*while (true) {

		char choice = cv::waitKey(10);

		if (choice == 'c' || choice == 'C') {
			param.calibrate(inputImg);
		}
		if (choice == 'q' || choice == 'Q') {
			break;
		}
	}*/

	return 0;
}