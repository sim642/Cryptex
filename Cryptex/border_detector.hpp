#ifndef BORDER_DETECTOR_H
#define BORDER_DETECTOR_H

#include "blob_finder.hpp"
#include <opencv2/opencv.hpp>
#include <vector>
#include <utility>

class border_detector
{
	public:
		typedef std::pair<cv::Vec2f, cv::Vec2f> line_t;
		typedef std::vector<line_t> lines_t;

		border_detector(blob_finder &nblobber);
		virtual ~border_detector();

		void detect(const cv::Mat &frame, lines_t &borders);

		static float dist_closest(const lines_t &borders, const cv::Point2f &p);

		void draw(cv::Mat &display);

	private:
		blob_finder &blobber;

		std::vector<cv::Vec4i> lines;
};

#endif // BORDER_DETECTOR_H
