#ifndef LAB_UTIL_H
#define LAB_UTIL_H

#include <chrono>
#include <cmath>
#include <random>

/**
 * @brief Namespace containing typedefs, objects and functions used in multiple files.
 */
namespace Util
{
	// Time related definitions
	// Every duration used in the project is expressed in milliseconds.
	using duration = std::chrono::milliseconds;
	using clock = std::chrono::high_resolution_clock;
	using time_point = clock::time_point;

	// Variables used for random numbers
	inline std::random_device random_device;
	inline std::mt19937 random_engine{random_device()};

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
	 * @brief Normalizes an angle in degrees.
	 * @return The equivalent angle in the range [0, 360)
	 */
	inline int normalize_angle(int angle) {
		return (angle % 360 + 360) % 360;
	}

	/**
	 * @brief Returns the squared euclidean distance between two points.
	 */
	inline double squared_distance(double x1, double y1, double x2, double y2) {
		return (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
	}

	/**
	 * @brief Returns the euclidean distance between two points.
	 */
	inline double distance(double x1, double y1, double x2, double y2) {
		return std::sqrt(squared_distance(x1, y1, x2, y2));
	}
} // namespace Util

#endif
