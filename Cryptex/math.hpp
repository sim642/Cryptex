#ifndef MATH_H
#define MATH_H

#include <cmath>
#include <opencv2/core/types.hpp>

template<typename T>
using range_t = std::pair<T, T>;

typedef std::pair<cv::Point2f, cv::Point2f> line_t;
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
inline float vec_angle(const cv::Point_<T> &vec)
{
	return std::atan2(vec.y, vec.x);
}

template<typename T>
inline cv::Point_<T> scale_to(const cv::Point_<T> &vec, T len)
{
	return (len * cv::norm(vec)) * vec;
}

cv::Point2f cam2rel(const cv::Point2f &cam, const cv::Size2i &size);
line_t cam2rel(const line_t &cam, const cv::Size2i &size);
cv::Point2f rel2cam(const cv::Point2f &rel, const cv::Size2i &size);
line_t rel2cam(const line_t &rel, const cv::Size2i &size);

cv::Point2f rect2pol(const cv::Point2f &rect);
cv::Point2f pol2rect(const cv::Point2f &pol);

float dist_line_point(const cv::Point2f &a, const cv::Vec2f &n, const cv::Point2f &p);
float dist_line_point(const cv::Point2f &a, const cv::Point2f &b, const cv::Point2f &p);
float dist_lineseg_point(const cv::Point2f &a, const cv::Point2f &b, const cv::Point2f &p);

cv::Point2f lengthen(const cv::Point2f &p, float dl);
line_t lengthen(const line_t &line, float dl);
cv::Point2f midpoint(const line_t &line);

#endif // MATH_H
