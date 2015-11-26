#include "border_detector.hpp"
#include "math.hpp"

using namespace std;

border_detector::border_detector(blob_finder &nblobber) : blobber(nblobber)
{
	blobber.opening = true;
}

border_detector::~border_detector()
{

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

	cv::Mat mask;
	blobber.threshold(frame, mask);

	cv::Mat canny;
	cv::Canny(mask, canny, 0, 255);

	cv::HoughLinesP(canny, lines, 1, CV_PI / 180, 30, 50, 100);
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
		mindist = min(mindist, dist_line_point(border.first, border.second, p));
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
	}
}
