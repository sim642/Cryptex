#include "blob_finder.hpp"
#include <vector>
#include <algorithm>
#include "global.hpp"
#include "math.hpp"

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

void blob_finder::detect(const cv::Mat &mask, keypoints_t &keypoints)
{
	detector->detect(mask, keypoints);
}

cv::KeyPoint blob_finder::largest(const keypoints_t &keypoints)
{
	auto largest = max_element(keypoints.begin(), keypoints.end(), [](const cv::KeyPoint &lhs, const cv::KeyPoint &rhs)
	{
		return lhs.size < rhs.size;
	});

	if (largest != keypoints.end())
		return *largest;
	else
		return none;
}

cv::KeyPoint blob_finder::largest(const cv::Mat &frame)
{
	cv::Mat mask;
	threshold(frame, mask);

	keypoints_t keypoints;
	detect(mask, keypoints);

	return largest(keypoints);
}

void blob_finder::angle_filter_out(keypoints_t& ps1, keypoints_t& ps2, float angle, float delta)
{
	vector<bool> keep1(ps1.size(), true), keep2(ps2.size(), true);

	for (unsigned int i = 0; i < ps1.size(); i++)
	{
		for (unsigned int j = 0; j < ps2.size(); j++)
		{
			auto d = ps2[j].pt - ps1[i].pt;
			float deg = rad2deg(vec_angle(d));
			if (fabs(deg - angle) < delta || fabs(deg - (-180 + angle)) < delta)
				keep1[i] = keep2[j] = false;
		}
	}

	keypoints_t nps1, nps2;
	for (unsigned int i = 0; i < ps1.size(); i++)
	{
		if (keep1[i])
			nps1.push_back(ps1[i]);
	}
	for (unsigned int j = 0; j < ps2.size(); j++)
	{
		if (keep2[j])
			nps2.push_back(ps2[j]);
	}

	ps1 = move(nps1);
	ps2 = move(nps2);
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
