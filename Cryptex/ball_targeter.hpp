#ifndef BALL_TARGETER_H
#define BALL_TARGETER_H

#include "blob_targeter.hpp"
#include "blob_tracker.hpp"
#include <functional>

class ball_targeter : public blob_targeter
{
	public:
		typedef std::function<float(const blob&)> scorer_t;

		ball_targeter(blob_finder &finder, int maxdist, scorer_t &nscorer, float gap);
		virtual ~ball_targeter();

		virtual boost::optional<blob> update(const cv::Mat &frame);
		virtual void reset();

		virtual void draw(cv::Mat &display);

	private:
		blob_tracker tracker;
		scorer_t &scorer;
		float gap;
		int ballid;
};

#endif // BALL_TARGETER_H
