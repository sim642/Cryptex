#ifndef BLOB_H
#define BLOB_H

#include <opencv2/opencv.hpp>

struct blob
{
	cv::KeyPoint kp;
	float factor, dist;
};

#endif // BLOB_H
