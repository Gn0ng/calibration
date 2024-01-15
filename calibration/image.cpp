#include "image.h"
#include <opencv2/opencv.hpp>
#include<vector>

image::image()
{
}

image::~image()
{
}

void image::addImg(cv::Mat image)
{

	images.push_back(image);
}

cv::Mat image::checkCorner3(cv::Mat img) {
	cv::Mat gray;
	cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
	cv::Mat mask = calGaussianKernel(ksize, sigma);

	cv::Mat dy, dx;
	cv::Sobel(gray, dx, CV_64F, 1, 0, 3);
	cv::Sobel(gray, dy, CV_64F, 0, 1, 3);

	cv::Mat A_01 = conv(dx.mul(dy), mask);
	cv::Mat A_00 = conv(dy.mul(dy), mask);
	cv::Mat A_11 = conv(dx.mul(dx), mask);

	A_01.convertTo(A_01, CV_64F);
	A_00.convertTo(A_00, CV_64F);
	A_11.convertTo(A_11, CV_64F);

	cv::Mat C;

	if (useDet) {
		cv::Mat deter_values = A_00.mul(A_11) - A_01.mul(A_01);

		if (deter_values.channels() == 3) {
			cv::cvtColor(deter_values, deter_values, cv::COLOR_BGR2GRAY);
		}

		C = deter_values - k * (A_00 + A_11).mul(A_00 + A_11);
	}
	else {
		cv::Mat eigens[3];
		cv::eigen(A_00, eigens[0]);
		cv::eigen(A_01, eigens[1]);
		cv::eigen(A_11, eigens[2]);

		// Find the maximum eigenvalue for each channel
		cv::Mat max_eigenval = cv::Mat::zeros(eigens[0].size(), CV_64F);
		for (int i = 0; i < 3; ++i) {
			cv::max(max_eigenval, eigens[i].col(0), max_eigenval);
		}

		C = eigens[0].col(0).mul(eigens[1].col(0)) - (k * (eigens[0].col(0) + eigens[1].col(0)).mul(eigens[0].col(0) + eigens[1].col(0)));
	}

	cv::Mat result = cv::Mat::zeros(gray.size(), CV_64F);
	cv::compare(C, threshold, result, cv::CMP_GT);
	cv::Mat copy = img.clone();
	cv::Mat copy2 = img.clone();
	cv::Mat copy3 = img.clone();
	//drawCircles(copy, result);
	//cv::imshow("first", copy);
	//cv::imwrite("first.png", copy);
	//cv::waitKey();
	cv::Mat cornersAfterMinDisCheck = minDisCheck(result);
	//drawCircles(copy2, cornersAfterMinDisCheck);
	//cv::imshow("second", copy2);
	//cv::imwrite("second.png", copy2);
	//cv::waitKey();


	cv::Mat final = findChess(gray, cornersAfterMinDisCheck);
	drawCircles(copy3, final);
	//cv::imwrite("final.png", copy3);
	cv::Mat realFinal = numbering(copy3,final);

	return realFinal;
	return copy3;
}


cv::Mat image::calGaussianKernel(int size, double sigma) {
	cv::Mat kernel = cv::getGaussianKernel(size, sigma);
	return kernel * kernel.t();
}



cv::Mat image::conv(const cv::Mat& img, const cv::Mat& kernel)
{
	cv::Mat result;
	cv::filter2D(img, result, -1, kernel, cv::Point(-1, -1), 0, cv::BORDER_CONSTANT);
	return result;
}



void image::drawCircles(cv::Mat& img, const cv::Mat& mask)
{
	std::vector<cv::Point> points;
	cv::findNonZero(mask, points);

	for (const cv::Point& point : points) {
		cv::circle(img, point, 5, cv::Scalar(0, 0, 255), 2);
	}
}

cv::Point2f image::getMean(const std::vector<cv::Point2f>& points)
{
	cv::Point2f mean(0, 0);

	for (const auto& point : points) {
		mean += point;
	}

	if (!points.empty()) {
		mean /= static_cast<float>(points.size());
	}

	return mean;
}


cv::Mat image::minDisCheck(const cv::Mat& result)
{
	cv::Mat mergedCorners = result.clone();

	for (int i = 0; i < result.rows; ++i) {
		for (int j = 0; j < result.cols; ++j) {
			if (result.at<uchar>(i, j) > 0) {
				std::vector<cv::Point2f> cornerPoints;

				// Collect nearby corner points within minDistance
				for (int ii = std::max(0, i - minDistance); ii <= std::min(result.rows - 1, i + minDistance); ++ii) {
					for (int jj = std::max(0, j - minDistance); jj <= std::min(result.cols - 1, j + minDistance); ++jj) {
						if (result.at<uchar>(ii, jj) > 0) {
							cornerPoints.emplace_back(jj, ii);
						}
					}
				}

				if (!cornerPoints.empty()) {
					// Merge nearby corner points by computing their mean
					cv::Point2f mergedPoint = getMean(cornerPoints);

					// Set the mean value for all merged locations
					for (const auto& corner : cornerPoints) {
						mergedCorners.at<uchar>(static_cast<int>(corner.y), static_cast<int>(corner.x)) = 0;
					}

					mergedCorners.at<uchar>(static_cast<int>(mergedPoint.y), static_cast<int>(mergedPoint.x)) = 255;
				}
			}
		}
	}

	return mergedCorners;
}

cv::Mat image::findChess(const cv::Mat& image, const cv::Mat& point)
{

	std::vector<cv::Point2f> cornerPoints;
	for (int i = 0; i < point.rows; ++i) {
		for (int j = 0; j < point.cols; ++j) {
			if (point.at<uchar>(i, j) > 0) {
				cornerPoints.emplace_back(j, i);
			}
		}
	}

	std::vector<cv::Point2f> chessPoints;
	chessPoints.reserve(cornerPoints.size());

	for (cv::Point2f cornerPoint : cornerPoints) {
		double quarter1 = 0, quarter2 = 0, quarter3 = 0, quarter4 = 0;

		if (cornerPoint.x > margin && cornerPoint.y > margin && cornerPoint.x < image.cols - margin && cornerPoint.y < image.rows - margin) {
			for (int i = 1; i <= windowSize; i++) {
				for (int j = 1; j <= windowSize; j++) {
					quarter1 += image.at<uchar>(cornerPoint.y - i, cornerPoint.x - j);
					quarter2 += image.at<uchar>(cornerPoint.y - i, cornerPoint.x + j);
					quarter3 += image.at<uchar>(cornerPoint.y + i, cornerPoint.x + j);
					quarter4 += image.at<uchar>(cornerPoint.y + i, cornerPoint.x - j);
				}
			}
			quarter1 /= 25;
			quarter2 /= 25;
			quarter3 /= 25;
			quarter4 /= 25;

			//std::cout << "x : " << cornerPoint.x << std::endl;
			//std::cout << "y : " << cornerPoint.y << std::endl;
			//std::cout << "quarter1 : " << quarter1 << std::endl;
			//std::cout << "quarter2 : " << quarter2 << std::endl;
			//std::cout << "quarter3 : " << quarter3 << std::endl;
			//std::cout << "quarter4 : " << quarter4 << std::endl;




			if ((quarter1 <= quarter && quarter2 >= quarter && quarter3 <= quarter && quarter4 >= quarter)
				|| (quarter1 >= quarter && quarter2 <= quarter && quarter3 >= quarter && quarter4 <= quarter)) {
				chessPoints.push_back(cornerPoint);
			}
		}
	}

	cv::Mat resultPoint = cv::Mat::zeros(image.size(), CV_8U);

	if (chessPoints.empty()) {
		std::cout << "못찾음" << std::endl;
	}
	else {
		std::cout << chessPoints.size();
		for (cv::Point2f chessPoint : chessPoints) {

			resultPoint.at<uchar>(chessPoint.y, chessPoint.x) = 255;
		}
	}

	return resultPoint;
}

//찾은 코너점들 70개인지 확인하고 0,0 좌표의 기준 찾기
cv::Mat image::numbering(const cv::Mat& image, const cv::Mat& point)
{	
	std::vector<cv::Point2f> points;
	for (int i = 0; i < point.rows; ++i) {
		for (int j = 0; j < point.cols; ++j) {
			if (point.at<uchar>(i, j) > 0) {
				points.emplace_back(j, i);
			}
		}
	}


	int x = 0, y = 0;
	for (cv::Point2f Cpoint : points) {
		for (int i = 0; i < point.rows; i++) {
			for (int j = 0; j < point.cols; j++) {
				if (i == Cpoint.x && j == Cpoint.y) {
					cv::String text = "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
					cv::Point textPoint;
					textPoint.x = i - 40;
					textPoint.y = j - 10;
					cv::putText(image, text, textPoint, 2, 0.3, cv::Scalar(0,0,255));
					y++;
					if (y == 10) {
						y = 0;
						x++;
					}
				}
			}
		}
	}
	return image;
}
