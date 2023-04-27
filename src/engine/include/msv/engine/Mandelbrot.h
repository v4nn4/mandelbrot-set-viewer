#pragma once

namespace msv
{
	class MandelbrotHelper
	{
	public:
		static double ComputeMandelbrotRadius(const double x0, const double y0);

	private:
		static double Radius(const double x, const double y);
		static void MandelbrotFunction(const double x0, const double y0, double &x, double &y);
	};
}