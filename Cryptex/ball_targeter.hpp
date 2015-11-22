#ifndef BALL_TARGETER_H
#define BALL_TARGETER_H

#include "blob_targeter.hpp"
#include "blob_tracker.hpp"

class ball_targeter : public blob_targeter
{
	public:
		ball_targeter(blob_finder &finder, int maxdist, float gap);
		virtual ~ball_targeter();

		virtual boost::optional<blob> update(const cv::Mat &frame);
		virtual void reset();

		virtual void draw(cv::Mat &display);

	private:
		blob_tracker tracker;
		float gap;
		int ballid;
};

#endif // BALL_TARGETER_H
