#include "coordinates.hpp"

void CoordinateSystemHelper::TransformInplace(const CoordinateSystem& coordinateSystem, double& x, double& y)
{
	x = coordinateSystem.x0 + (-0.5 + x)*coordinateSystem.width;
	y = coordinateSystem.y0 + (-0.5 + y)*coordinateSystem.height;
}

void CoordinateSystemHelper::ZoomCoordinateSystem(CoordinateSystem& coordinateSystem, const double zoomFactor)
{
	auto aspectRatio = coordinateSystem.width / coordinateSystem.height;
	coordinateSystem.width = coordinateSystem.width / zoomFactor;
	coordinateSystem.height = coordinateSystem.width / aspectRatio;
}

void CoordinateSystemHelper::TranslateCoordinateSystem(CoordinateSystem& coordinateSystem, const double x, const double y)
{
	coordinateSystem.x0 += static_cast<double>(x)*coordinateSystem.width;
	coordinateSystem.y0 += static_cast<double>(y)*coordinateSystem.height;
}