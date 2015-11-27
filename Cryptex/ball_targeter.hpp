#ifndef BALL_TARGETER_H
#define BALL_TARGETER_H

#include "blob_targeter.hpp"
#include "blob_tracker.hpp"
#include "border_detector.hpp"
#include "blob_modifier.hpp"
#include <functional>
#include <vector>

class ball_targeter : public blob_targeter
{
	public:
		typedef std::function<float(const blob&)> scorer_t;

		ball_targeter(blob_finder &finder, int maxdist, scorer_t &nscorer, float gap);
		virtual ~ball_targeter();

		virtual boost::optional<blob> update(const cv::Mat &frame);
		virtual void reset();

		virtual void draw(cv::Mat &display);

		void add_modifier(blob_modifier &modifier);

	private:
		blob_tracker tracker;
		std::vector<blob_modifier*> modifiers;
		scorer_t &scorer;
		float gap;
		int ballid;
};

#endif // BALL_TARGETER_H
