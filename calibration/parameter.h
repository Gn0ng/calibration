#pragma once
#include <vector>
#include "image.h"
class parameter
{
public:

	parameter();
	~parameter();

	double getFx();
	double getFy();
	double getCx();
	double getCy();
	double getSkew_c();
	std::vector<double> getR();
	std::vector<double> getT();

	void calibrate(image image);

private:

	double fx;
	double fy;
	double cx;
	double cy;
	double skew_c;
	std::vector<double> r;
	std::vector<double> t;

	void getParameter();
	void getDistortion();
};
