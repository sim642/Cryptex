#ifndef BLOB_FINDER_H
#define BLOB_FINDER_H

#include <opencv2/opencv.hpp>
#include <string>

class blob_finder
{
	public:
		typedef cv::Scalar_<int> bounds_t;
		static const cv::KeyPoint none;

		blob_finder();
		blob_finder(const std::string &filename);
		virtual ~blob_finder();

		void init_detector(const cv::SimpleBlobDetector::Params &params);
		void load_file(const std::string &filename);

		cv::KeyPoint largest(const cv::Mat &frame);
	private:
		bounds_t lower, upper;
		cv::Ptr<cv::SimpleBlobDetector> detector;
};

#endif // BLOB_FINDER_H
