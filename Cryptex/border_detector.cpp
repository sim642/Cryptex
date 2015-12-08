#include "border_detector.hpp"
#include "math.hpp"
#include "global.hpp"

using namespace std;

border_detector::border_detector(blob_finder &nblobber) : blobber(nblobber)
{
	//blobber.opening = true;
	default_lines();
}

border_detector::border_detector(blob_finder &nblobber, const std::string &lines_name) : blobber(nblobber)
{
	//blobber.opening = true;
	//load_lines(lines_name);
	default_lines();
}

border_detector::~border_detector()
{

}

void border_detector::default_lines()
{
	cannythres = {0, 255};
	linethres = 50;
	linelength = 160;
	linegap = 17;
}

void border_detector::load_lines(const std::string& lines_name)
{
	default_lines();

	cv::FileStorage fs(global::calib_filename(lines_name), cv::FileStorage::READ);
	fs["mincanny"] >> cannythres.first;
	fs["maxcanny"] >> cannythres.second;
	fs["thres"] >> linethres;
	fs["length"] >> linelength;
	fs["gap"] >> linegap;
}

void border_detector::save_lines(const std::string& lines_name)
{
	cv::FileStorage fs(global::calib_filename(lines_name), cv::FileStorage::WRITE);
	fs << "mincanny" << cannythres.first;
	fs << "maxcanny" << cannythres.second;
	fs << "thres" << linethres;
	fs << "length" << linelength;
	fs << "gap" << linegap;
}

void border_detector::detect(const cv::Mat &frame)
{
	lines_t oborders;
	detect(frame, oborders);
}

void border_detector::detect(const cv::Mat &frame, lines_t &oborders)
{
	lines.clear();
	borders.clear();

	blobber.threshold(frame, mask);

	cv::Canny(mask, canny, cannythres.first, cannythres.second);

	cv::HoughLinesP(canny, lines, 1, CV_PI / 180, linethres, linelength, linegap);
	for (auto &line : lines)
	{
		borders.push_back(line_t(cam2rel(cv::Point2f(line[0], line[1]), mask.size()), cam2rel(cv::Point2f(line[2], line[3]), mask.size())));
	}

	oborders = borders;
}

float border_detector::dist_closest(const lines_t& borders, const cv::Point2f& p)
{
	float mindist = numeric_limits<float>::max();
	for (auto &border : borders)
	{
		auto l = border.second - border.first;
		auto b = p - border.first;
		auto cross = l.x * b.y - l.y * b.x;
		mindist = min(mindist, dist_line_point(border.first, border.second, p) * -sign(cross));
	}
	return mindist;
}

float border_detector::dist_closest(const cv::Point2f& p)
{
	return dist_closest(borders, p);
}

void border_detector::draw(cv::Mat &display)
{
	for (auto &l : lines)
	{
		cv::line(display, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(255, 0, 0), 3, CV_AA);
		cv::circle(display, cv::Point(l[0], l[1]), 3, cv::Scalar(0, 0, 255));
	}
}

void border_detector::modify(blob& b)
{
	b.borderdist = dist_closest(b.rel);
}
