#ifndef BLOB_TARGETER_H
#define BLOB_TARGETER_H

#include "blob_finder.hpp"
#include <opencv2/opencv.hpp>
#include <boost/optional.hpp>

class blob_targeter
{
	public:
		blob_targeter(blob_finder &nfinder);
		virtual ~blob_targeter();

		virtual boost::optional<blob> update(const cv::Mat &frame);
		virtual void reset();

		virtual void draw(cv::Mat &display);

	protected:
		blob_finder &finder;
};

#endif // BLOB_TARGETER_H
