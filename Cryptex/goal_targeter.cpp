#include "goal_targeter.hpp"
#include <limits>
#include <algorithm>
#include <functional>
#include <iterator>

using namespace std;

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

	blob_finder::angle_filter_out(goals, goals2, enemys, 90, delta);

	mygoals.clear();
	othergoals.clear();
	transform(goals.begin(), goals.end(), back_inserter(mygoals), bind(goal_targeter::blob2line, placeholders::_1, frame.size()));
	transform(goals2.begin(), goals2.end(), back_inserter(othergoals), bind(goal_targeter::blob2line, placeholders::_1, frame.size()));

	return target = blob_finder::largest(goals);
}

void goal_targeter::draw(cv::Mat& display)
{
	for (auto &b : goals)
	{
		if (b == *target)
			cv::rectangle(display, b.rect, cv::Scalar(255, 255, 0), 5);
		else
			cv::rectangle(display, b.rect, cv::Scalar(255, 255, 0), 1);
	}

	for (auto &b : goals2)
		cv::rectangle(display, b.rect, cv::Scalar(0, 255, 255), 1);

	for (auto &b : enemys)
		cv::rectangle(display, b.rect, cv::Scalar(0, 0, 255), 1);
}

void goal_targeter::modify(blob& b)
{
	b.enemydist = dist_closest(enemys, b.rel);
	b.goaldist = min(dist_closest(mygoals, b.rel), dist_closest(othergoals, b.rel));
}

line_t goal_targeter::blob2line(const blob& b, const cv::Size2i &size)
{
	//return cam2rel(line_t(b.rect.tl() + cv::Point(0, b.rect.height), b.rect.br()), size);
	return cam2rel(line_t(b.rect.tl() + cv::Point(0, b.rect.height), b.rect.tl() + cv::Point(b.rect.width, b.rect.height)), size);
}

float goal_targeter::dist_closest(const blobs_t& bs, const cv::Point2f& p)
{
	float mindist = numeric_limits<float>::max();
	for (auto &b : bs)
	{
		mindist = min<float>(mindist, cv::norm(b.rel - p));
	}
	return mindist;
}

float goal_targeter::dist_closest(const lines_t& ls, const cv::Point2f& p)
{
	float mindist = numeric_limits<float>::max();
	for (auto &l : ls)
	{
		mindist = min(mindist, dist_lineseg_point(l.first, l.second, p));
	}
	return mindist;
}
