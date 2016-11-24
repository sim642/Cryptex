#ifndef CAMERA_H
#define CAMERA_H

#include <opencv2/opencv.hpp>
#include <vector>
#include "math.hpp"

class camera
{
	public:
		camera(int new_id);
		virtual ~camera();

		void load_camera(const std::string &name);
		void save_camera(const std::string &name);

		void update();

		cv::Mat frame;

		cv::Point2f cam2rel(const cv::Point2f &cam) const;
		line_t cam2rel(const line_t &cam) const;
		cv::Point2f rel2cam(const cv::Point2f &rel) const;
		line_t rel2cam(const line_t &rel) const;

		friend class camera_module;

	private:
		int id;
		cv::VideoCapture capture;

		cv::Size2i size;
		float hfov, vfov, h, alpha, theta;
};

typedef std::vector<camera> multi_camera;

#endif // CAMERA_H
