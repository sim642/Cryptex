#include "border_detector.hpp"
#include "math.hpp"
#include "global.hpp"

using namespace std;

border_detector::border_detector(blob_finder &nblobber, blob_finder &nblobber2) : blobber(nblobber), blobber2(nblobber2)
{
	//blobber.opening = true;
	default_lines();
}

border_detector::border_detector(blob_finder &nblobber, blob_finder &nblobber2, const std::string &lines_name) : blobber(nblobber), blobber2(nblobber2)
{
	//blobber.opening = true;
	load_lines(lines_name);
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
void border_detector::detect(const camera &cam)
{
	lines_t oborders;
	detect(cam, oborders);
}

void border_detector::detect(const camera &cam, lines_t &oborders)
{
	lines.clear();
	borders.clear();

	cv::Mat mask;
	threshold(cam, mask);

	cv::Canny(mask, canny, cannythres.first, cannythres.second);

	cv::HoughLinesP(canny, lines, 1, CV_PI / 180, linethres, linelength, linegap);
	for (auto &line : lines)
	{
		borders.push_back(line_t{cam.cam2rel(cv::Point2f(line[0], line[1])),
		                         cam.cam2rel(cv::Point2f(line[2], line[3])),
		                         cam.i});
	}

	oborders = borders;
}

void border_detector::detect(const multi_camera &cams)
{
	lines_t oborders;
	detect(cams, oborders);
}

void border_detector::detect(const multi_camera &cams, lines_t &oborders)
{
	lines.clear();
	borders.clear();

	for (auto &cam : cams)
	{
		cv::Mat mask;
		threshold(cam, mask);

		cv::Canny(mask, canny, cannythres.first, cannythres.second);

		std::vector<cv::Vec4i> lines2;
		cv::HoughLinesP(canny, lines2, 1, CV_PI / 180, linethres, linelength, linegap);

		for (auto &line : lines2)
		{
            lines.push_back(line);
			borders.push_back(line_t{cam.cam2rel(cv::Point2f(line[0], line[1])),
			                         cam.cam2rel(cv::Point2f(line[2], line[3])),
			                         cam.i});
		}
	}

	oborders = borders;
}

float border_detector::dist_closest(const lines_t& borders, const cv::Point2f& p)
{
	float mindist = numeric_limits<float>::max();
	for (auto &border : borders)
	{
		mindist = min(mindist, -sdist_line_point(border.first, border.second, p));
	}
	return mindist;
}

float border_detector::dist_closest(const cv::Point2f& p)
{
	return dist_closest(borders, p);
}

void border_detector::draw(cv::Mat &display)
{
	for (size_t i = 0; i < lines.size(); i++)
	{
		auto &l = lines[i];
		cv::line(display, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(255, 0, 0), 3, CV_AA);
		cv::circle(display, cv::Point(l[0], l[1]), 3, cv::Scalar(0, 0, 255));
	}
}

void border_detector::draw(cv::Mat &multi_display, const multi_camera &cams)
{
	for (size_t i = 0; i < lines.size(); i++)
	{
		auto &l = lines[i];
		auto display = display4cam(multi_display, cams, borders[i].cam);
		cv::line(display, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(255, 0, 0), 3, CV_AA);
		cv::circle(display, cv::Point(l[0], l[1]), 3, cv::Scalar(0, 0, 255));
	}
}

void border_detector::modify(blob& b)
{
	b.borderdist = dist_closest(b.rel);
}

void border_detector::threshold(const camera &cam, cv::Mat &mask)
{
    cv::Mat m1, m2;
    blobber.threshold(cam, m1);
    blobber2.threshold(cam, m2);

    cv::dilate(m1, m1, cv::getStructuringElement(CV_SHAPE_ELLIPSE, cv::Size(7, 7)));
    cv::dilate(m2, m2, cv::getStructuringElement(CV_SHAPE_ELLIPSE, cv::Size(7, 7)));

    cv::bitwise_and(m1, m2, mask);
    //blobber.threshold(cam, mask);
}
