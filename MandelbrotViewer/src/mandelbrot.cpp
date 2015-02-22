#include "mandelbrot.hpp"

void MandelbrotHelper::MandelbrotFunction(const double x0, const double y0, double& x, double& y)
{
	auto xtemp = x;
	x = x*x-y*y+x0;
	y = 2*xtemp*y+y0;
}

double MandelbrotHelper::Radius(const double x, const double y)
{
	return x*x+y*y;
}

double MandelbrotHelper::ComputeMandelbrotRadius(const double x0, const double y0)
{
	auto x = x0;
	auto y = y0;
	auto iterations = 0;
	while(Radius(x,y)<=4 && iterations<50)
	{
		MandelbrotFunction(x0, y0, x, y);
		iterations++;
	}
	return Radius(x, y);
}