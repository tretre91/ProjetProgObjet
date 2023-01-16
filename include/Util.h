#ifndef LAB_UTIL_H
#define LAB_UTIL_H

#include <chrono>
#include <cmath>

namespace Util
{
	// Time related definitions
	// Every duration used in the project is expressed in milliseconds.
	using duration = std::chrono::milliseconds;
	using clock = std::chrono::high_resolution_clock;
	using time_point = clock::time_point;

	// Utilitary functions
	/**
	 * @brief Converts an angle from degrees to radians.
	 */
	inline double deg_to_rad(int angle) {
		return angle * (M_PI / 180.0);
	}

	/**
	 * @brief Converts an angle from radians to degrees.
	 */
	inline int rad_to_deg(double angle) {
		return std::round(angle * (180.0 / M_PI));
	}

	/**
	 * @brief Normalizes an angle in degrees
	 * @return The equivalent angle in the range [0, 360)
	 */
	inline int normalize_angle(int angle) {
		return (angle % 360 + 360) % 360;
	}

	/**
	 * @brief Returns the euclidean distance between two points.
	 */
	inline double distance(double x1, double y1, double x2, double y2) {
		const double dx = x2 - x1;
		const double dy = y2 - y1;
		return std::sqrt(dx * dx + dy * dy);
	}
} // namespace Util

#endif
