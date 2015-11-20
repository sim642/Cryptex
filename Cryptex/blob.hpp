#ifndef BLOB_H
#define BLOB_H

#include <opencv2/opencv.hpp>
#include <vector>

struct blob
{
	cv::KeyPoint kp;
	float factor, dist;
	float score;
};

typedef std::vector<blob> blobs_t;

#endif // BLOB_H
