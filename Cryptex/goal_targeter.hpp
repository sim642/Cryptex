#ifndef GOAL_TARGETER_H
#define GOAL_TARGETER_H

#include "blob_targeter.hpp"
#include "blob_modifier.hpp"
#include <vector>

class goal_targeter : public blob_targeter, public blob_modifier
{
	public:
		goal_targeter(blob_finder &finder, blob_finder &nfinder2, float ndelta);
		virtual ~goal_targeter();

		virtual boost::optional<blob> update(const cv::Mat &frame);

		virtual void draw(cv::Mat &display);

		virtual void modify(blob &b);

		static line_t blob2line(const blob &b, const cv::Size2i &size);
	private:
		static float dist_closest(const blobs_t &bs, const cv::Point2f &p);
		static float dist_closest(const lines_t &ls, const cv::Point2f &p);

		blob_finder &finder2;
		float delta;

		blobs_t goals, goals2, enemys;
		boost::optional<blob> target;

		lines_t mygoals, othergoals;
};

#endif // GOAL_TARGETER_H
