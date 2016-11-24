#ifndef CAMERA_H
#define CAMERA_H

#include <opencv2/opencv.hpp>
#include <vector>

class camera
{
	public:
		camera(int new_id);
		virtual ~camera();

		void update();

		cv::Mat frame;

		cv::Point2f cam2rel(const cv::Point2f &cam, const cv::Size2i &size) const;
		line_t cam2rel(const line_t &cam, const cv::Size2i &size) const;
		cv::Point2f rel2cam(const cv::Point2f &rel, const cv::Size2i &size) const;
		line_t rel2cam(const line_t &rel, const cv::Size2i &size) const;

	private:
		int id;
		cv::VideoCapture capture;
};

typedef std::vector<camera> multi_camera;

#endif // CAMERA_H
