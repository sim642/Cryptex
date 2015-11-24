#ifndef BLOB_FINDER_H
#define BLOB_FINDER_H

#include "blob.hpp"
#include "blob_detector.hpp"
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

		blob_finder();
		blob_finder(const std::string &color_name);
		blob_finder(const std::string &color_name, const std::string &params_name);
		virtual ~blob_finder();

		void load_color(const std::string &color_name);
		void save_color(const std::string &color_name);
		void set_color(const bounds_t &new_lower, const bounds_t &new_upper);

		void threshold(const cv::Mat &frame, cv::Mat &mask);
		void detect(const cv::Mat &mask, blobs_t &blobs);
		void detect_frame(const cv::Mat &frame, blobs_t &blobs);
		static boost::optional<blob> largest(const blobs_t &blobs);

		static void angle_filter_out(blobs_t &bs1, blobs_t &bs2, float angle, float delta);

		friend class calibrator_window;

		blob_detector detector;

	private:
		bounds_t lower, upper;
		int struct_size;
};

#endif // BLOB_FINDER_H
