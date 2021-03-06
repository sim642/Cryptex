#ifndef MATH_H
#define MATH_H

#include <cmath>
#include <opencv2/core/types.hpp>

template<typename T>
using range_t = std::pair<T, T>;

struct line_t
{
	cv::Point2f first;
	cv::Point2f second;
	size_t cam;
};
typedef std::vector<line_t> lines_t;


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
inline T sqr(T x)
{
	return std::pow(x, 2);
}

template<typename T>
inline T clamp(T val, range_t<T> range)
{
	return std::min(range.second, std::max(range.first, val));
}

float ease_nexpn(float t, const cv::Point2f &p);

template<typename T>
inline T acos2(T val)
{
	//return asin(sqrt(1 - sqr(val)));
	T ret = acos(val);
	if (val >= 0)
		return ret;
	else
		return ret - M_PI;
}

template<typename T>
inline float vec_angle(const cv::Point_<T> &vec)
{
	return std::atan2(vec.y, vec.x);
}

template<typename T>
inline float vec_angle(const cv::Point_<T> &vec1, const cv::Point_<T> &vec2)
{
	/// v1 . v2 = |v1||v2|cos a
	return acos(vec1.dot(vec2) / (cv::norm(vec1) * cv::norm(vec2)));
}

template<typename T>
inline float vec_sangle(const cv::Point_<T> &vec1, const cv::Point_<T> &vec2)
{
	/// v1 * v2 = |v1||v2|sin a
	return asin(vec1.cross(vec2) / (cv::norm(vec1) * cv::norm(vec2)));
}

template<typename T>
inline cv::Point_<T> scale_to(const cv::Point_<T> &vec, T len)
{
	return (len * cv::norm(vec)) * vec;
}

cv::Point2f rect2pol(const cv::Point2f &rect);
cv::Point2f pol2rect(const cv::Point2f &pol);
cv::Point2f rotate(const cv::Point2f &vec, float deg);

float dist_line_point(const cv::Point2f &a, const cv::Vec2f &n, const cv::Point2f &p);
float dist_line_point(const cv::Point2f &a, const cv::Point2f &b, const cv::Point2f &p);
float sdist_line_point(const cv::Point2f &a, const cv::Point2f &b, const cv::Point2f &p);
float dist_lineseg_point(const cv::Point2f &a, const cv::Point2f &b, const cv::Point2f &p);

cv::Point2f lengthen(const cv::Point2f &p, float dl);
line_t lengthen(const line_t &line, float dl);
line_t lengthen_rel(const line_t &line, float r);
cv::Point2f midpoint(const line_t &line);

#endif // MATH_H
