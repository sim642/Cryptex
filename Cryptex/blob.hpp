#ifndef BLOB_H
#define BLOB_H

#include <opencv2/opencv.hpp>
#include <vector>

struct blob
{
	// frame attributes
	float area;
	cv::Point2f center;
	float radius;
	cv::Rect rect;

	// physical attributes
	cv::Point2f rel;
	float dist, angle;

	// other attribues
	float score;
};

inline bool operator== (const blob &lhs, const blob &rhs)
{
	return lhs.center == rhs.center;
}

typedef std::vector<blob> blobs_t;

#endif // BLOB_H
