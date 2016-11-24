#ifndef CAMERA_H
#define CAMERA_H

#include <opencv2/opencv.hpp>
#include <vector>
#include "math.hpp"
#include <boost/ptr_container/ptr_vector.hpp>

class camera
{
	public:
		camera(const std::string &new_name, bool open_now = true);
		virtual ~camera();

		bool open();

		void load_camera();
		void save_camera();

		void update();

		cv::Mat frame;

		cv::Point2f cam2rel(const cv::Point2f &cam) const;
		line_t cam2rel(const line_t &cam) const;
		cv::Point2f rel2cam(const cv::Point2f &rel) const;
		line_t rel2cam(const line_t &rel) const;

		friend class camera_module;

	private:
		const std::string name;

		std::string path;
		cv::VideoCapture capture;

		cv::Size2i size;
		float hfov, vfov, h, alpha, theta;
};

typedef boost::ptr_vector<camera> multi_camera;

multi_camera load_multi_camera();

#endif // CAMERA_H
