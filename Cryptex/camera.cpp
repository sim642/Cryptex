#include "camera.hpp"
#include "global.hpp"
#include "math.hpp"
#include <vector>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

using namespace std;

camera::camera(const std::string &new_name, bool open_now) : name(new_name)
{
	load_camera();
	if (open_now)
		open();
}

camera::~camera()
{

}

bool camera::open()
{
	return capture.open(path);
}

void camera::load_camera()
{
	cv::FileStorage fs(global::camera_filename(name), cv::FileStorage::READ);
	fs["hfov"] >> hfov;
	fs["vfov"] >> vfov;
	fs["h"] >> h;
	fs["alpha"] >> alpha;
	fs["theta"] >> theta;
	fs["path"] >> path;
}

void camera::save_camera()
{
	cv::FileStorage fs(global::camera_filename(name), cv::FileStorage::WRITE);
	fs << "hfov" << hfov;
	fs << "vfov" << vfov;
	fs << "h" << h;
	fs << "alpha" << alpha;
	fs << "theta" << theta;
	fs << "path" << path;
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

	return rotate(cv::Point2f(dx, dy), theta);
}

line_t camera::cam2rel(const line_t& cam) const
{
	return line_t(cam2rel(cam.first), cam2rel(cam.second));
}

cv::Point2f camera::rel2cam(const cv::Point2f &rel2) const
{
	cv::Point2f rel = rotate(rel2, -theta);

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


multi_camera load_multi_camera()
{
	vector<string> parts;
	boost::algorithm::split(parts, global::cams, boost::algorithm::is_any_of(","));

	multi_camera cams;
	for (auto &name : parts)
		cams.emplace_back(name);
	return cams;
}
