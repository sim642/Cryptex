#ifndef BLOB_FINDER_H
#define BLOB_FINDER_H

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

class calibrator_window;

class blob_finder
{
	public:
		typedef cv::Scalar_<int> bounds_t;
		static const cv::KeyPoint none;

		blob_finder();
		blob_finder(const std::string &filename);
		blob_finder(const std::string &filename1, const std::string &filename2);
		virtual ~blob_finder();

		void load_color(const std::string &filename);
		void save_color(const std::string &filename);
		void set_color(const bounds_t &new_lower, const bounds_t &new_upper);

		void load_params(const std::string &filename);
		void save_params(const std::string &filename);
		void set_params(const cv::SimpleBlobDetector::Params &new_params);

		void threshold(const cv::Mat &frame, cv::Mat &mask);
		void detect(const cv::Mat &mask, std::vector<cv::KeyPoint> &keypoints);
		cv::KeyPoint largest(const cv::Mat &frame);

		friend class calibrator_window;
	private:
		void init_detector();

		bounds_t lower, upper;
		int struct_size;
		cv::SimpleBlobDetector::Params params;

		cv::Ptr<cv::SimpleBlobDetector> detector;
};

#endif // BLOB_FINDER_H
