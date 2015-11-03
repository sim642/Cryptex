#include "blob_finder.hpp"
#include <vector>
#include "global.hpp"

using namespace std;

const cv::KeyPoint blob_finder::none(-1, -1, -1);

blob_finder::blob_finder()
{

}

blob_finder::blob_finder(const std::string &color_name) : blob_finder()
{
	load_color(color_name);
}

blob_finder::blob_finder(const std::string &color_name, const std::string &params_name) : blob_finder()
{
	load_color(color_name);
	load_params(params_name);
}

blob_finder::~blob_finder()
{

}

void blob_finder::load_color(const std::string &color_name)
{
	cv::FileStorage fs(global::env_filename(color_name), cv::FileStorage::READ);
	fs["lower"] >> lower;
	fs["upper"] >> upper;
	cv::read(fs["struct_size"], struct_size, 11);
}

void blob_finder::save_color(const std::string &color_name)
{
	cv::FileStorage fs(global::env_filename(color_name), cv::FileStorage::WRITE);
	fs << "lower" << lower;
	fs << "upper" << upper;
	fs << "struct_size" << struct_size;
}

void blob_finder::set_color(const bounds_t &new_lower, const bounds_t &new_upper)
{
	lower = new_lower;
	upper = new_upper;
}

void blob_finder::load_params(const std::string &params_name)
{
	cv::FileStorage fs(global::calib_filename(params_name), cv::FileStorage::READ);
	params.read(fs.root());
	init_detector();
}

void blob_finder::save_params(const std::string &params_name)
{
	cv::FileStorage fs(global::calib_filename(params_name), cv::FileStorage::WRITE);
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

boost::optional<blob_finder::factordist_t> blob_finder::factordist(const cv::Mat &frame, const cv::KeyPoint& largest)
{
	if (largest.size >= 0.f) // if blob found
	{
		int diff = frame.cols / 2 - largest.pt.x;
		float factor = float(diff) / (frame.cols / 2);

		float dist = (frame.rows - largest.pt.y) / float(frame.rows);

		return make_pair(factor, dist);
	}
	else
		return boost::none;
}

void blob_finder::init_detector()
{
	detector = cv::SimpleBlobDetector::create(params);
}
