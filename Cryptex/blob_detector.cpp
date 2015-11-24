#include "blob_detector.hpp"
#include "global.hpp"
#include <limits>

using namespace std;

blob_detector::blob_detector()
{
	default_params();
}

blob_detector::blob_detector(const std::string &params_name)
{
	load_params(params_name);
}

blob_detector::~blob_detector()
{

}

void blob_detector::default_params()
{
	area = {0.f, numeric_limits<float>::max()};
}

void blob_detector::load_params(const std::string &params_name)
{
	default_params();

	cv::FileStorage fs(global::calib_filename(params_name), cv::FileStorage::READ);
	fs["minarea"] >> area.first;
	fs["maxarea"] >> area.second;
}

void blob_detector::save_params(const std::string &params_name)
{
	cv::FileStorage fs(global::calib_filename(params_name), cv::FileStorage::WRITE);
	fs << "minarea" << area.first;
	fs << "maxarea" << area.second;
}

void blob_detector::detect(const cv::Mat& mask, std::vector<blob>& blobs)
{
	blobs.clear();

	vector<vector<cv::Point>> contours;
	cv::findContours(mask.clone(), contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);

	for (auto &contour : contours)
	{
		blob b;

		auto moms = cv::moments(cv::Mat(contour));

		if (moms.m00 == 0.0)
			continue;

		b.area = moms.m00;
		if (b.area < area.first || b.area > area.second)
			continue;

		b.center = cv::Point2f(moms.m10 / moms.m00, moms.m01 / moms.m00);
		if (mask.at<uchar>(cvRound(b.center.y), cvRound(b.center.x)) != (uchar)255) // WTF y before x
			continue;

		b.radius = radius(contour, b.center);
		b.rect = cv::boundingRect(contour);

		blobs.push_back(b);
	}
}

float blob_detector::radius(const std::vector<cv::Point> &contour, const cv::Point &center)
{
	vector<float> dists;
	for (auto &p : contour)
		dists.push_back(cv::norm(p - center));

	sort(dists.begin(), dists.end());
	float radius = (dists[(dists.size() - 1) / 2] + dists[dists.size() / 2]) / 2;
	return radius;
}
