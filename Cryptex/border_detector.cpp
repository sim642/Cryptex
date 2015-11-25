#include "border_detector.hpp"

border_detector::border_detector(blob_finder &nblobber) : blobber(nblobber)
{
	blobber.opening = true;
}

border_detector::~border_detector()
{

}

bool border_detector::detect(const cv::Mat &frame)
{
	cv::Mat mask;
	blobber.threshold(frame, mask);

	cv::Point2f sum(0, 0);
	int cnt = 0;
	for (int y = 0; y < mask.size().height; y += 10)
	{
		for (int x = 0; x < mask.size().width; x += 10)
		{
			if (mask.at<uchar>(y, x))
			{
				auto pol = rect2pol(cam2rel(cv::Point2f(x, y), mask.size()));
				sum += pol;
				cnt++;
			}
		}
	}

	auto avg = sum / cnt;

	return avg.x / 3.5 < 1.0;
}
