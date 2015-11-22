#include "math.hpp"
#include "global.hpp"

using namespace std;

cv::Point2f cam2rel(const cv::Point2f &cam, const cv::Size2i &size)
{
	float phi = global::vfov * (0.5f - cam.y / size.height) + global::alpha;
	float dx = global::h * tan(deg2rad(phi));

	float psi = global::hfov * (0.5f - cam.x / size.width);
	float dy = sqrt(sqr(global::h) + sqr(dx)) * tan(deg2rad(psi));

	return cv::Point2f(dx, dy);
}

cv::Point2f rel2cam(const cv::Point2f &rel, const cv::Size2i &size)
{
	float phi = rad2deg(atan(rel.x / global::h));
	float y = (0.5f - (phi - global::alpha) / global::vfov) * size.height;

	float psi = rad2deg(atan(rel.y / sqrt(sqr(global::h) + sqr(rel.x))));
	float x = (0.5f - psi / global::hfov) * size.width;

	return cv::Point2f(x, y);
}

cv::Point2f rect2pol(const cv::Point2f &rect)
{
	return cv::Point2f(cv::norm(rect), rad2deg(vec_angle(rect)));
}

cv::Point2f pol2rect(const cv::Point2f &pol)
{
	return pol.x * cv::Point2f(cos(deg2rad(pol.y)), sin(deg2rad(pol.y)));
}

float dist_line_point(const cv::Point2f &a, const cv::Vec2f &n, const cv::Point2f &p)
{
	/// https://en.wikipedia.org/wiki/Distance_from_a_point_to_a_line#Vector_formulation
	cv::Point2f proj = (a - p).dot(n) * n;
	return cv::norm((a - p) - proj);
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
