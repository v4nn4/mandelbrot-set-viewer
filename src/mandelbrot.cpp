#include "mandelbrot.hpp"
#include <cmath>

void TransformInplace(const CoordinateSystem& coordinateSystem, double& x, double& y)
{
	x = coordinateSystem.x0+(-0.5+x)*coordinateSystem.width;
	y = coordinateSystem.y0+(-0.5+y)*coordinateSystem.height;
}

void ZoomCoordinateSystem(CoordinateSystem& coordinateSystem, const double zoomFactor)
{
	double aspectRatio = coordinateSystem.width/coordinateSystem.height;
	coordinateSystem.width = coordinateSystem.width/zoomFactor;
	coordinateSystem.height = coordinateSystem.width/aspectRatio;
}

void TranslateCoordinateSystem(CoordinateSystem& coordinateSystem, const double x, const double y)
{
	coordinateSystem.x0 += (double)x*coordinateSystem.width;
	coordinateSystem.y0 += (double)y*coordinateSystem.height;
}

void MandelbrotFunction(const double x0, const double y0, double& x, double& y)
{
	double xtemp = x;
	x = x*x-y*y+x0;
	y = 2*xtemp*y+y0;
}

double Radius(const double x, const double y)
{
	return x*x+y*y;
}

double ComputeMandelbrotRadius(const double x0, const double y0)
{
	double x = x0;
	double y = y0;
	int iterations = 0;
	while(Radius(x,y)<=4 && iterations<50)
	{
		MandelbrotFunction(x0,y0,x,y);
		iterations++;
	}
	return Radius(x,y);
}