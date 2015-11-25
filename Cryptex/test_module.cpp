#include "test_module.hpp"

#include <iostream>
#include <boost/asio.hpp>
#include <chrono>
#include <thread>

#include "util.hpp"

#include "global.hpp"
#include "device_id.hpp"

#include "rs485_dongle.hpp"
#include "driver.hpp"
#include "main_controller.hpp"
#include "serial_controller.hpp"

#include "calibrator_window.hpp"
#include "blob_finder.hpp"
#include "blob_tracker.hpp"

using namespace std;

test_module::test_module()
{

}

test_module::~test_module()
{

}

module::type test_module::run(const module::type &prev_module)
{
	cv::VideoCapture capture(global::video_id);

	cv::namedWindow("walls");

	blob_finder blobber("valge");

	while (1)
	{
		cv::Mat frame;
		capture >> frame;

		cv::Mat mask;
		blobber.threshold(frame, mask);

		cv::Mat display;
		cv::cvtColor(mask, display, CV_GRAY2BGR);

		cv::Point2f sum(0, 0);
		int cnt = 0;
		for (int y = 0; y < mask.size().height; y += 10)
		{
			for (int x = 0; x < mask.size().width; x += 10)
			{
				if (mask.at<uchar>(y, x))
				{
					auto pol = rect2pol(cam2rel(cv::Point2f(x, y), mask.size()));
					sum += pol;
					cnt++;
				}
			}
		}

		auto avg = sum / cnt;
		cout << avg.x / 3.5 << endl;
		auto rel = pol2rect(cv::Point2f(avg.x / 3.5, avg.y * 1.5));
		auto cam = rel2cam(rel, display.size());

		cv::circle(display, cam, 5.f, cv::Scalar(255, 0, 0));
		//cv::line(display, cv::Point(0, avg.x * 50), cv::Point(display.size().width, avg.x * 50), cv::Scalar(0, 0, 255));

		cv::imshow("walls", display);

		char key = cv::waitKey(1000 / 30);
		switch (key)
		{
			case 'q':
				return module::type::menu;
		}
	}

	return module::type::menu;
}
