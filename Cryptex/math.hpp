#ifndef MATH_H
#define MATH_H

#include <cmath>

inline float deg2rad(float deg)
{
	return deg * M_PI / 180;
}

inline float rad2deg(float rad)
{
	return rad * 180 / M_PI;
}

template<typename T>
int sign(T val)
{
	if (val > 0)
		return 1;
	else if (val < 0)
		return -1;
	else
		return 0;
}

#endif // MATH_H
