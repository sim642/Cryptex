#ifndef BORDER_DETECTOR_H
#define BORDER_DETECTOR_H

#include "blob_finder.hpp"
#include "blob_modifier.hpp"
#include <opencv2/opencv.hpp>
#include <vector>
#include <utility>

class linecalibrator_module;

class border_detector : public blob_modifier
{
	public:
		border_detector(blob_finder &nblobber);
		border_detector(blob_finder &nblobber, const std::string &lines_name);
		virtual ~border_detector();

		void default_lines();
		void load_lines(const std::string &lines_name);
		void save_lines(const std::string &lines_name);

		void detect(const cv::Mat &frame);
		void detect(const cv::Mat &frame, lines_t &borders);

		static float dist_closest(const lines_t &borders, const cv::Point2f &p);
		float dist_closest(const cv::Point2f &p);

		void draw(cv::Mat &display);

		virtual void modify(blob &b);

		friend class linecalibrator_module;

	private:
		blob_finder &blobber;
		range_t<int> cannythres;
		int linethres, linelength, linegap;

		cv::Mat mask, canny;

		std::vector<cv::Vec4i> lines;
		lines_t borders;
};

#endif // BORDER_DETECTOR_H
