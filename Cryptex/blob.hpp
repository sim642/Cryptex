#ifndef BLOB_H
#define BLOB_H

#include <opencv2/opencv.hpp>
#include <vector>

struct blob
{
	cv::KeyPoint kp;
	float factor, dist;
	cv::Point2f rel;
	float score;
};

inline bool operator== (const blob &lhs, const blob &rhs)
{
	return lhs.kp.pt == rhs.kp.pt;
}

typedef std::vector<blob> blobs_t;

#endif // BLOB_H
