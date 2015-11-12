#ifndef BLOB_FINDER_H
#define BLOB_FINDER_H

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include <boost/optional.hpp>
#include <utility>

class calibrator_window;

class blob_finder
{
	public:
		typedef cv::Scalar_<int> bounds_t;
		static const cv::KeyPoint none;
		typedef std::pair<float, float> factordist_t;

		blob_finder();
		blob_finder(const std::string &color_name);
		blob_finder(const std::string &color_name, const std::string &params_name);
		virtual ~blob_finder();

		void load_color(const std::string &color_name);
		void save_color(const std::string &color_name);
		void set_color(const bounds_t &new_lower, const bounds_t &new_upper);

		void load_params(const std::string &params_name);
		void save_params(const std::string &params_name);
		void set_params(const cv::SimpleBlobDetector::Params &new_params);

		void threshold(const cv::Mat &frame, cv::Mat &mask);
		void detect(const cv::Mat &mask, std::vector<cv::KeyPoint> &keypoints);
		static cv::KeyPoint largest(const std::vector<cv::KeyPoint> &keypoints);
		cv::KeyPoint largest(const cv::Mat &frame);

		boost::optional<factordist_t> factordist(const cv::Mat &frame, const cv::KeyPoint &largest);

		friend class calibrator_window;
	private:
		void init_detector();

		bounds_t lower, upper;
		int struct_size;
		cv::SimpleBlobDetector::Params params;

		cv::Ptr<cv::SimpleBlobDetector> detector;
};

#endif // BLOB_FINDER_H
