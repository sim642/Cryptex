#include "blob_finder.hpp"
#include <vector>
#include <algorithm>
#include "global.hpp"
#include "math.hpp"

using namespace std;

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

void blob_finder::detect(const cv::Mat &mask, blobs_t &blobs)
{
	keypoints_t keypoints;
	detect(mask, keypoints);

	blobs.clear();
	for (auto &kp : keypoints)
	{
		blob b;
		b.kp = kp;
		tie(b.factor, b.dist) = factordist(mask, b);

		blobs.push_back(b);
	}
}

void blob_finder::detect_frame(const cv::Mat& frame, blobs_t& blobs)
{
	cv::Mat mask;
	threshold(frame, mask);
	detect(mask, blobs);
}

boost::optional<blob> blob_finder::largest(const blobs_t &blobs)
{
	auto largest = max_element(blobs.begin(), blobs.end(), [](const blob &lhs, const blob &rhs)
	{
		return lhs.kp.size < rhs.kp.size;
	});

	if (largest != blobs.end())
		return *largest;
	else
		return boost::none;
}

void blob_finder::angle_filter_out(blobs_t& bs1, blobs_t &bs2, float angle, float delta)
{
	vector<bool> keep1(bs1.size(), true), keep2(bs2.size(), true);

	for (unsigned int i = 0; i < bs1.size(); i++)
	{
		for (unsigned int j = 0; j < bs2.size(); j++)
		{
			auto d = bs2[j].kp.pt - bs1[i].kp.pt;
			float deg = rad2deg(vec_angle(d));
			if (fabs(deg - angle) < delta || fabs(deg - (-180 + angle)) < delta)
				keep1[i] = keep2[j] = false;
		}
	}

	blobs_t nbs1, nbs2;
	for (unsigned int i = 0; i < bs1.size(); i++)
	{
		if (keep1[i])
			nbs1.push_back(bs1[i]);
	}
	for (unsigned int j = 0; j < bs2.size(); j++)
	{
		if (keep2[j])
			nbs2.push_back(bs2[j]);
	}

	bs1 = move(nbs1);
	bs2 = move(nbs2);
}

blob_finder::factordist_t blob_finder::factordist(const cv::Mat &frame, const blob& largest)
{
	int diff = frame.cols / 2 - largest.kp.pt.x;
	float factor = float(diff) / (frame.cols / 2);

	float dist = (frame.rows - largest.kp.pt.y) / float(frame.rows);

	return make_pair(factor, dist);
}

void blob_finder::init_detector()
{
	detector = cv::SimpleBlobDetector::create(params);
}
