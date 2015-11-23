#ifndef MATH_H
#define MATH_H

#include <cmath>
#include <opencv2/core/types.hpp>

template<typename T>
using range_t = std::pair<T, T>;

inline float deg2rad(float deg)
{
	return deg * M_PI / 180;
}

inline float rad2deg(float rad)
{
	return rad * 180 / M_PI;
}

template<typename T>
inline int sign(T val)
{
	if (val > 0)
		return 1;
	else if (val < 0)
		return -1;
	else
		return 0;
}

template<typename T>
inline T clamp(T val, range_t<T> range)
{
	return std::min(range.second, std::max(range.first, val));
}

template<typename T>
inline float vec_angle(const cv::Point_<T> &vec)
{
	return atan2(vec.y, vec.x);
}

#endif // MATH_H
