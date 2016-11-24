#include "camera.hpp"
#include "global.hpp"
#include "math.hpp"

camera::camera(int new_id) : id(new_id), capture(id)
{

}

camera::~camera()
{

}

void camera::update()
{
	capture >> frame;
}

cv::Point2f camera::cam2rel(const cv::Point2f &cam, const cv::Size2i &size) const
{
	float phi = global::vfov * (0.5f - cam.y / size.height) + global::alpha;
	float dx = global::h * tan(deg2rad(phi));

	float psi = global::hfov * (0.5f - cam.x / size.width);
	float dy = sqrt(sqr(global::h) + sqr(dx)) * tan(deg2rad(psi));

	return cv::Point2f(dx, dy);
}

line_t camera::cam2rel(const line_t& cam, const cv::Size2i& size) const
{
	return line_t(cam2rel(cam.first, size), cam2rel(cam.second, size));
}

cv::Point2f camera::rel2cam(const cv::Point2f &rel, const cv::Size2i &size) const
{
	float phi = rad2deg(atan(rel.x / global::h));
	float y = (0.5f - (phi - global::alpha) / global::vfov) * size.height;

	float psi = rad2deg(atan(rel.y / sqrt(sqr(global::h) + sqr(rel.x))));
	float x = (0.5f - psi / global::hfov) * size.width;

	return cv::Point2f(x, y);
}

line_t camera::rel2cam(const line_t& rel, const cv::Size2i& size) const
{
	return line_t(rel2cam(rel.first, size), rel2cam(rel.second, size));
}
