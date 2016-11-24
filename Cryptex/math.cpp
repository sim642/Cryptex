#include "math.hpp"
#include "global.hpp"

using namespace std;

cv::Point2f rect2pol(const cv::Point2f &rect)
{
	return cv::Point2f(cv::norm(rect), rad2deg(vec_angle(rect)));
}

cv::Point2f pol2rect(const cv::Point2f &pol)
{
	return pol.x * cv::Point2f(cos(deg2rad(pol.y)), sin(deg2rad(pol.y)));
}

cv::Point2f rotate(const cv::Point2f &vec, float deg)
{
	cv::Point2f pol = rect2pol(vec);
	pol.y += deg;
	return pol2rect(pol);
}

float ease_nexpn(float t, const cv::Point2f &p)
{
	//return 1 - exp(-(log(1 - p.y) / -p.x) * t);
	return -expm1((log1p(-p.y) / p.x) * t);
}

float dist_line_point(const cv::Point2f &a, const cv::Vec2f &n, const cv::Point2f &p)
{
	/// https://en.wikipedia.org/wiki/Distance_from_a_point_to_a_line#Vector_formulation
	cv::Point2f proj = (a - p).dot(n) * n;
	return cv::norm((a - p) - proj);
}

float dist_line_point(const cv::Point2f& a, const cv::Point2f& b, const cv::Point2f& p)
{
	cv::Point2f nn = b - a; // not actually unit vector
	float t = (p - a).dot(nn) / (nn.dot(nn));
	return cv::norm((a + t * nn) - p);
}

float sdist_line_point(const cv::Point2f& a, const cv::Point2f& b, const cv::Point2f& p)
{
	return dist_line_point(a, b, p) * sign(vec_sangle(b - a, p - a));
}

float dist_lineseg_point(const cv::Point2f &a, const cv::Point2f &b, const cv::Point2f &p)
{
	/// http://www.geometrictools.com/Documentation/DistancePointLine.pdf
	cv::Point2f nn = b - a; // not actually unit vector
	float t = (p - a).dot(nn) / (nn.dot(nn));
	if (t <= 0)
		return cv::norm(a - p);
	else if (t >= 1)
		return cv::norm(b - p);
	else
		return cv::norm((a + t * nn) - p);
}

cv::Point2f lengthen(const cv::Point2f &p, float dl)
{
	return (1.f + dl / cv::norm(p)) * p;
}

line_t lengthen(const line_t &line, float dl)
{
	auto diff = line.second - line.first;
	auto p = line.first + scale_to(diff, -dl);
	auto d = lengthen(diff, 2 * dl);
	return line_t(p, p + d);
}

cv::Point2f midpoint(const line_t& line)
{
	return (line.first + line.second) / 2.f;
}
