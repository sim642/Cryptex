#include "camera_module.hpp"

#include <opencv2/opencv.hpp>
#include <iostream>
#include <functional>

#include "global.hpp"
#include "math.hpp"

using namespace std;

camera_module::camera_module()
{

}

camera_module::~camera_module()
{

}

void mousecb(int event, int x, int y, int flags, void *userdata)
{
	cv::Size &framesize = *static_cast<cv::Size*>(userdata);
	if (event == cv::EVENT_LBUTTONUP)
	{
		cout << cam2rel({x, y}, framesize) << endl;
	}
}

module::type camera_module::run(const module::type &prev_module)
{
	cv::VideoCapture capture(global::video_id);

	cv::namedWindow("camera");

	int hfov = global::hfov, vfov = global::vfov, h = global::h * 1000, alpha = global::alpha;

	cv::createTrackbar("hfov", "camera", &hfov, 100);
	cv::createTrackbar("vfov", "camera", &vfov, 100);
	cv::createTrackbar("h", "camera", &h, 1000);
	cv::createTrackbar("alpha", "camera", &alpha, 90);

	cv::Size framesize;
	{
		cv::Mat frame;
		capture >> frame;
		framesize = frame.size();
	}

	cv::setMouseCallback("camera", &mousecb, &framesize);

	while (1)
	{
		cv::Mat frame;
		capture >> frame;

		cv::Mat display;
		frame.copyTo(display);

		global::hfov = hfov;
		global::vfov = vfov;
		global::h = h / 1000.f;
		global::alpha = alpha;

		float delta = 0.2f;
		for (float dx = 0.f; dx <= 10.f; dx += delta)
		{
			for (float dy = -3.f; dy <= 3.f; dy += delta)
			{
				auto pos = rel2cam({dx, dy}, frame.size());
				cv::circle(display, pos, 2.f, cv::Scalar(0, 0, 255));
			}
		}

		cv::imshow("camera", display);

		char key = cv::waitKey(1000 / 30);
		switch (key)
		{
			case 'q':
				return module::type::menu;
		}
	}

	return module::type::menu;
}
