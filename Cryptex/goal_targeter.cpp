#include "goal_targeter.hpp"

goal_targeter::goal_targeter(blob_finder &finder, blob_finder &nfinder2, float ndelta) : blob_targeter(finder), finder2(nfinder2), delta(ndelta)
{

}

goal_targeter::~goal_targeter()
{

}

boost::optional<blob> goal_targeter::update(const cv::Mat& frame)
{
	/*cv::Mat framelow;
	cv::resize(frame, framelow, cv::Size(), scalelow, scalelow, CV_INTER_AREA);*/

	finder.detect_frame(frame, goals);
	finder2.detect_frame(frame, goals2);

	blob_finder::angle_filter_out(goals, goals2, 90, delta);

	return target = blob_finder::largest(goals);
}

void goal_targeter::draw(cv::Mat& display)
{
	for (auto &b : goals)
	{
		if (b == *target)
			cv::rectangle(display, b.rect, cv::Scalar(255, 255, 0), 5);
		else
			cv::rectangle(display, b.rect, cv::Scalar(255, 255, 0), 3);
	}

	for (auto &b : goals2)
		cv::rectangle(display, b.rect, cv::Scalar(0, 255, 255), 3);
}
