#pragma once

struct CoordinateSystem
{
	double x0;
	double y0;
	double width;
	double height;
};

void TransformInplace(const CoordinateSystem& system, double& x, double& y);
void ZoomCoordinateSystem(CoordinateSystem& system, const double zoomFactor);
void TranslateCoordinateSystem(CoordinateSystem& system, const double x, const double y);
void MandelbrotFunction(const double x0, const double y0, double& x, double& y);
double Radius(const double x, const double y);
double ComputeMandelbrotRadius(const double x0, const double y0);