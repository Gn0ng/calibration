#pragma once
#include <vector>
#include <opencv2/opencv.hpp>
class image
{
public:
	image();
	~image();

	void addImg(cv::Mat image);
	cv::Mat checkCorner3(cv::Mat chessBoard);


private:

	std::vector<cv::Mat> images;
	double k = 0.04;
	bool useDet = true;
	double threshold = 30000000.0;
	int minDistance = 10;
	int pixelDistance = 10;
	int ksize = 3;
	double sigma = 1.0;
	const int CHECKERBOARD[2]{ 10, 7 };
	double quarter = 50;
	const int windowSize = 5;
	const int margin = 5;
	int minPoints = CHECKERBOARD[0] * CHECKERBOARD[1];

	cv::Mat calGaussianKernel(int size, double sigma);
	cv::Mat conv(const cv::Mat& img, const cv::Mat& kernel);
	void drawCircles(cv::Mat& img, const cv::Mat& mask);
	cv::Point2f getMean(const std::vector<cv::Point2f>& points);
	cv::Mat minDisCheck(const cv::Mat& result);
	cv::Mat findChess(const cv::Mat& image, const cv::Mat& point);
	cv::Mat numbering(const cv::Mat& image, const cv::Mat& point);
	cv::Point2f findOrigin(const std::vector<cv::Point2f>& points, int rotate);
	float distance(const cv::Point2f& p1, const cv::Point2f& p2);
	bool isNext(const std::vector<cv::Point2f>& points, const cv::Point2f& p1);

};