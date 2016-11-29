#ifndef BLOB_DETECTOR_H
#define BLOB_DETECTOR_H

#include "blob.hpp"
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include "math.hpp"
#include "camera.hpp"

class calibrator_window;

class blob_detector
{
	public:
		blob_detector();
		blob_detector(const std::string &params_name);
		virtual ~blob_detector();

		void default_params();
		void load_params(const std::string &params_name);
		void save_params(const std::string &params_name);

		void detect(const cv::Mat &mask, std::vector<blob> &blobs);

		friend class calibrator_window;

	private:
		float radius(const std::vector<cv::Point> &contour, const cv::Point &center);

		range_t<float> area;
};

#endif // BLOB_DETECTOR_H
