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
	static cv::Point2f prev(0, 0);

	camera &cam = *static_cast<camera*>(userdata);
	if (event == cv::EVENT_LBUTTONUP)
	{
		auto rel = cam.cam2rel(cv::Point2f(x, y));
		auto pol = rect2pol(rel);
		cout << rel << " " << pol << " " << cv::norm(rel - prev) << endl;

		prev = rel;
	}
}

module::type camera_module::run(const module::type &prev_module)
{
	camera cam(2);
	std::string name = "pseyef";
	cam.load_camera(name);

	cv::namedWindow("camera");

	int hfov = cam.hfov * 10, vfov = cam.vfov * 10, h = cam.h * 1000, alpha = cam.alpha * 10, theta = cam.theta * 10;

	cv::createTrackbar("hfov", "camera", &hfov, 1000);
	cv::createTrackbar("vfov", "camera", &vfov, 1000);
	cv::createTrackbar("h", "camera", &h, 1000);
	cv::createTrackbar("alpha", "camera", &alpha, 900);
	cv::createTrackbar("theta", "camera", &theta, 3600);

	cv::setMouseCallback("camera", &mousecb, &cam);

	while (1)
	{
		cam.update();

		cv::Mat display;
		cam.frame.copyTo(display);

		cam.hfov = hfov / 10.f;
		cam.vfov = vfov / 10.f;
		cam.h = h / 1000.f;
		cam.alpha = alpha / 10.f;
		cam.theta = theta / 10.f;

		float delta = 0.1f;
		for (float dx = -10.f; dx <= 10.f; dx += delta)
		{
			for (float dy = -10.f; dy <= 10.f; dy += delta)
			{
				auto pos = cam.rel2cam({dx, dy});
				cv::circle(display, pos, 2.f, cv::Scalar(0, 0, 255));
			}
		}

		cv::imshow("camera", display);

		char key = cv::waitKey(1000 / 30);
		switch (key)
		{
			case 'q':
			{
				cam.save_camera(name);
				return module::type::menu;
			}
		}
	}

	return module::type::menu;
}
