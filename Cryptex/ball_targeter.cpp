#include "ball_targeter.hpp"
#include <string>

using namespace std;

ball_targeter::ball_targeter(blob_finder &finder, int maxdist, float ngap) : blob_targeter(finder), tracker(maxdist), gap(ngap), ballid(0)
{

}

ball_targeter::~ball_targeter()
{

}

boost::optional<blob> ball_targeter::update(const cv::Mat& frame)
{
	blobs_t balls;
	finder.detect_frame(frame, balls);
	tracker.update(balls);

	int bestid = tracker.best();
	if (!bestid || !ballid || !tracker[ballid] || (tracker[bestid]->score + gap < tracker[ballid]->score))
		ballid = bestid;

	return tracker[ballid];
}

void ball_targeter::reset()
{
	tracker.clear();
	ballid = 0;
}

void ball_targeter::draw(cv::Mat &display)
{
	for (auto &p : tracker.get_all())
	{
		int id = p.first;
		const blob &b = p.second;

		cv::circle(display, b.kp.pt, b.kp.size / 2, cv::Scalar(0, 255, id == ballid ? 255 : 0), 2);
		cv::putText(display, to_string(id), b.kp.pt, cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(0, 0, 255), 1);
		cv::putText(display, to_string(b.score), b.kp.pt + cv::Point2f(0, 15), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(255, 0, 0), 1);
	}
}

