#include "test_module.hpp"

#include <iostream>
#include <boost/asio.hpp>
#include <chrono>
#include <thread>
#include <vector>
#include <cctype>

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
#include "border_detector.hpp"

using namespace std;

test_module::test_module()
{

}

test_module::~test_module()
{

}

module::type test_module::run(const module::type &prev_module)
{
	vector<string> files =
	{
		"my_front_view.jpg",
		"close-border.jpg",
		"mid-border.jpg",
		"far-border.jpg"
	};

	vector<cv::Mat> images;
	for (auto &file : files)
		images.push_back(cv::imread("pics/" + file));

	cv::namedWindow("walls");
	cv::namedWindow("edges");

	blob_finder blobber("valge");
	blobber.opening = true;

	int i = 0;
	while (1)
	{
		cv::Mat frame;
		images[i].copyTo(frame);

		cv::Mat mask;
		blobber.threshold(frame, mask);

		cv::Mat color(mask.size(), CV_8UC3);
		color.setTo(cv::Scalar(255, 0, 255));

		cv::Mat display;
		frame.copyTo(display);
		color.copyTo(display, mask);

		cv::Mat canny;
		cv::Canny(mask, canny, 0, 255);

		vector<cv::Vec4i> lines;
		cv::HoughLinesP(canny, lines, 1, CV_PI/180, 30, 50, 100);
		for (size_t i = 0; i < lines.size(); i++)
		{
			cv::Vec4i l = lines[i];
			cv::line(display, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(255, 0, 0), 3, CV_AA);
		}

		cv::imshow("walls", display);
		cv::imshow("edges", canny);

		char key = cv::waitKey(1000 / 30);
		switch (key)
		{
			case 'q':
				return module::type::menu;

			case 's':
				cv::imwrite("pics/ss.jpg", display);
				break;

			default:
				if (isdigit(key))
				{
					i = key - '1';
				}
				break;
		}
	}

	return module::type::menu;
}
