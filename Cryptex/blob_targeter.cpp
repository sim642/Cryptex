#include "blob_targeter.hpp"

blob_targeter::blob_targeter(blob_finder &nfinder) : finder(nfinder)
{

}

blob_targeter::~blob_targeter()
{

}

boost::optional<blob> blob_targeter::update(const cv::Mat& frame)
{
	blobs_t blobs;
	finder.detect_frame(frame, blobs);
	return blob_finder::largest(blobs);
}

void blob_targeter::reset()
{

}

void blob_targeter::draw(cv::Mat &display)
{

}
