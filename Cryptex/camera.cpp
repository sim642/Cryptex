#include "camera.hpp"
#include "global.hpp"
#include "math.hpp"

camera::camera(int new_id) : id(new_id), capture(id)
{

}

camera::~camera()
{

}

void camera::load_camera(const std::string &name)
{
	cv::FileStorage fs(global::camera_filename(name), cv::FileStorage::READ);
	fs["hfov"] >> hfov;
	fs["vfov"] >> vfov;
	fs["h"] >> h;
	fs["alpha"] >> alpha;
	fs["theta"] >> theta;
}

void camera::save_camera(const std::string &name)
{
	cv::FileStorage fs(global::camera_filename(name), cv::FileStorage::WRITE);
	fs << "hfov" << hfov;
	fs << "vfov" << vfov;
	fs << "h" << h;
	fs << "alpha" << alpha;
	fs << "theta" << theta;
}

void camera::update()
{
	capture >> frame;
	size = frame.size();
}

cv::Point2f camera::cam2rel(const cv::Point2f &cam) const
{
	float phi = vfov * (0.5f - cam.y / size.height) + alpha;
	float dx = h * tan(deg2rad(phi));

	float psi = hfov * (0.5f - cam.x / size.width);
	float dy = sqrt(sqr(h) + sqr(dx)) * tan(deg2rad(psi));

	return cv::Point2f(dx, dy);
}

line_t camera::cam2rel(const line_t& cam) const
{
	return line_t(cam2rel(cam.first), cam2rel(cam.second));
}

cv::Point2f camera::rel2cam(const cv::Point2f &rel) const
{
	float phi = rad2deg(atan(rel.x / h));
	float y = (0.5f - (phi - alpha) / vfov) * size.height;

	float psi = rad2deg(atan(rel.y / sqrt(sqr(h) + sqr(rel.x))));
	float x = (0.5f - psi / hfov) * size.width;

	return cv::Point2f(x, y);
}

line_t camera::rel2cam(const line_t& rel) const
{
	return line_t(rel2cam(rel.first), rel2cam(rel.second));
}
