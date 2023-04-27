#pragma once

namespace msv
{
	struct CoordinateSystem
	{
		double x0;
		double y0;
		double width;
		double height;
	};

	class CoordinateSystemHelper
	{
	public:
		static void TransformInplace(const CoordinateSystem &system, double &x, double &y);
		static void ZoomCoordinateSystem(CoordinateSystem &system, const double zoomFactor);
		static void TranslateCoordinateSystem(CoordinateSystem &system, const double x, const double y);
	};
}