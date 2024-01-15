#include "parameter.h"


parameter::parameter()
{
}

parameter::~parameter()
{
}

double parameter::getFx()
{
    return fx;
}

double parameter::getFy()
{
    return fy;
}

double parameter::getCx()
{
    return cx;
}

double parameter::getCy()
{
    return cy;
}

double parameter::getSkew_c()
{
    return skew_c;
}

std::vector<double> parameter::getR()
{
    return r;
}

std::vector<double> parameter::getT()
{
    return t;
}

void parameter::calibrate(image image)
{



}

void parameter::getParameter()
{
}

void parameter::getDistortion()
{
}