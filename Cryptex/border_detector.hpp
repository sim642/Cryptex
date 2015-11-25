#ifndef BORDER_DETECTOR_H
#define BORDER_DETECTOR_H

#include "blob_finder.hpp"
#include <opencv2/opencv.hpp>

class border_detector
{
	public:
		border_detector(blob_finder &nblobber);
		virtual ~border_detector();

		bool detect(const cv::Mat &frame);

	private:
		blob_finder &blobber;
};

#endif // BORDER_DETECTOR_H
