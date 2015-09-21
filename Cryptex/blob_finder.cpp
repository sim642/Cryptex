#include "blob_finder.hpp"
#include <vector>

using namespace std;

const cv::KeyPoint blob_finder::none(-1, -1, -1);

blob_finder::blob_finder()
{

}

blob_finder::blob_finder(const std::string &filename) : blob_finder()
{
	load_color(filename);
}

blob_finder::blob_finder(const std::string &filename1, const std::string &filename2) : blob_finder()
{
	load_color(filename1);
	load_params(filename2);
}

blob_finder::~blob_finder()
{

}

void blob_finder::load_color(const std::string &filename)
{
	cv::FileStorage fs(filename, cv::FileStorage::READ);
	fs["lower"] >> lower;
	fs["upper"] >> upper;
	cv::read(fs["struct_size"], struct_size, 11);
}

void blob_finder::save_color(const std::string &filename)
{
	cv::FileStorage fs(filename, cv::FileStorage::WRITE);
	fs << "lower" << lower;
	fs << "upper" << upper;
	fs << "struct_size" << struct_size;
}

void blob_finder::set_color(const bounds_t &new_lower, const bounds_t &new_upper)
{
	lower = new_lower;
	upper = new_upper;
}

void blob_finder::load_params(const std::string &filename)
{
	cv::FileStorage fs(filename, cv::FileStorage::READ);
	params.read(fs.root());
	init_detector();
}

void blob_finder::save_params(const std::string &filename)
{
	cv::FileStorage fs(filename, cv::FileStorage::WRITE);
	params.write(fs);
}

void blob_finder::set_params(const cv::SimpleBlobDetector::Params &new_params)
{
	params = new_params;
	init_detector();
}

void blob_finder::threshold(const cv::Mat &frame, cv::Mat &mask)
{
	cv::Mat hsv;
	cv::cvtColor(frame, hsv, CV_BGR2HSV);

	cv::inRange(hsv, cv::Scalar(lower), cv::Scalar(upper), mask);

	auto structuring = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(struct_size, struct_size));
	cv::dilate(mask, mask, structuring);
	cv::erode(mask, mask, structuring);
}

void blob_finder::detect(const cv::Mat &mask, std::vector<cv::KeyPoint> &keypoints)
{
	detector->detect(mask, keypoints);
}

cv::KeyPoint blob_finder::largest(const cv::Mat &frame)
{
	cv::Mat mask;
	threshold(frame, mask);

	vector<cv::KeyPoint> keypoints;
	detect(mask, keypoints);

	auto largest = max_element(keypoints.begin(), keypoints.end(), [](const cv::KeyPoint &lhs, const cv::KeyPoint &rhs)
	{
		return lhs.size < rhs.size;
	});

	if (largest != keypoints.end())
		return *largest;
	else
		return none;
}

void blob_finder::init_detector()
{
	detector = cv::SimpleBlobDetector::create(params);
}
