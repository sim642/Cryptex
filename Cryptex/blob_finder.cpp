#include "blob_finder.hpp"
#include <vector>
#include <algorithm>
#include "global.hpp"
#include "math.hpp"

using namespace std;

blob_finder::blob_finder() : opening(false)
{

}

blob_finder::blob_finder(const std::string &color_name) : blob_finder()
{
	load_color(color_name);
}

blob_finder::blob_finder(const std::string &color_name, const std::string &params_name) : blob_finder()
{
	load_color(color_name);
	detector.load_params(params_name);
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

void blob_finder::threshold(const cv::Mat &frame, cv::Mat &mask)
{
	cv::Mat hsv;
	cv::cvtColor(frame, hsv, CV_BGR2HSV);

	cv::inRange(hsv, cv::Scalar(lower), cv::Scalar(upper), mask);

	auto structuring = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(struct_size, struct_size));

	cv::morphologyEx(mask, mask, opening ? cv::MORPH_OPEN : cv::MORPH_CLOSE, structuring);
}

void blob_finder::detect(const cv::Mat &mask, blobs_t &blobs)
{
	detector.detect(mask, blobs);

	for (auto &b : blobs)
	{
		b.rel = cam2rel(b.center, mask.size());
		auto pol = rect2pol(b.rel);
		b.dist = pol.x;
		b.angle = pol.y;
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
		return lhs.radius < rhs.radius;
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
			auto d = bs2[j].center - bs1[i].center;
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
