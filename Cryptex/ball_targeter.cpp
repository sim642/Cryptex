#include "ball_targeter.hpp"
#include <string>

using namespace std;

ball_targeter::ball_targeter(blob_finder &finder, int maxdist, blob_modifier &nmodifier, scorer_t &nscorer, float ngap) : blob_targeter(finder), tracker(maxdist), modifier(nmodifier), scorer(nscorer), gap(ngap), ballid(0)
{

}

ball_targeter::~ball_targeter()
{

}

boost::optional<blob> ball_targeter::update(const multi_camera& cams)
{
	blobs_t balls;
	finder.detect_frame(cams, balls);
	tracker.update(balls);

	for (auto &p : tracker.get_all())
		modifier.modify(p.second);

	for (auto &p : tracker.get_all())
	{
		blob &b = p.second;
		b.score = scorer(b);
	}

	int bestid = tracker.best();
	if (!bestid || !ballid || !tracker[ballid] || (tracker[bestid]->score + gap < tracker[ballid]->score))
	{
		if (bestid && tracker[bestid]->score < 1000.f)
			ballid = bestid;
		else if (!bestid)
			ballid = bestid;
	}
	else if (ballid && tracker[ballid]->score > 1000.f)
		ballid = 0;

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

		cv::circle(display, b.center, b.radius, cv::Scalar(0, 255, id == ballid ? 255 : 0), 2);
		cv::putText(display, to_string(id), b.center, cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(0, 0, 255), 1);
		cv::putText(display, to_string(b.score), b.center + cv::Point2f(0, 15), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(255, 0, 0), 1);
	}
}
