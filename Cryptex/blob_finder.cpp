#include "blob_finder.hpp"
#include <vector>

using namespace std;

const cv::KeyPoint blob_finder::none(-1, -1, -1);

blob_finder::blob_finder()
{

}

blob_finder::blob_finder(const std::string &filename) : blob_finder()
{
	load_file(filename);
}

blob_finder::~blob_finder()
{

}

void blob_finder::init_detector(const cv::SimpleBlobDetector::Params &params)
{
	detector = cv::SimpleBlobDetector::create(params);
}

void blob_finder::load_file(const std::string &filename)
{
	cv::FileStorage fs(filename, cv::FileStorage::READ);
	fs["lower"] >> lower;
	fs["upper"] >> upper;
}

void blob_finder::save_file(const std::string &filename)
{
	cv::FileStorage fs(filename, cv::FileStorage::WRITE);
	fs << "lower" << lower;
	fs << "upper" << upper;
}

void blob_finder::threshold(const cv::Mat &frame, cv::Mat &mask)
{
	cv::Mat hsv;
	cv::cvtColor(frame, hsv, CV_BGR2HSV);

	cv::inRange(hsv, cv::Scalar(lower), cv::Scalar(upper), mask);

	auto structuring = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(11, 11));
	cv::dilate(mask, mask, structuring);
	cv::erode(mask, mask, structuring);
}

cv::KeyPoint blob_finder::largest(const cv::Mat &frame)
{
	cv::Mat mask;
	threshold(frame, mask);

	vector<cv::KeyPoint> keypoints;
	detector->detect(mask, keypoints);

	auto largest = max_element(keypoints.begin(), keypoints.end(), [](const cv::KeyPoint &lhs, const cv::KeyPoint &rhs)
	{
		return lhs.size < rhs.size;
	});

	if (largest != keypoints.end())
		return *largest;
	else
		return none;
}
