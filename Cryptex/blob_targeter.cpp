#include "blob_targeter.hpp"

blob_targeter::blob_targeter(blob_finder &nfinder) : finder(nfinder)
{

}

blob_targeter::~blob_targeter()
{

}

boost::optional<blob> blob_targeter::update(const multi_camera& cams)
{
	blobs_t blobs;
	finder.detect_frame(cams, blobs);
	return blob_finder::largest(blobs);
}

void blob_targeter::reset()
{

}

void blob_targeter::draw(cv::Mat &display)
{

}
