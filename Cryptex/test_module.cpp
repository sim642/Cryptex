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
	cv::VideoCapture capture(global::video_id);

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

	cv::namedWindow("borders");

	blob_finder col1("col1"), col2("col2");

	int i = 0;
	while (1)
	{
		cv::Mat frame;
		//images[i].copyTo(frame);
		capture >> frame;

		cv::Mat display;
		frame.copyTo(display);

		/*cv::Mat mask1, mask2;
		col1.threshold(frame, mask1);
		col2.threshold(frame, mask2);

		cv::Sobel(mask1, mask1, -1, 1, 1);
		cv::Sobel(mask2, mask2, -1, 1, 1);

		cv::threshold(mask1, mask1, 128, 255, cv::THRESH_BINARY);
		cv::threshold(mask2, mask2, 128, 255, cv::THRESH_BINARY);

		cv::Mat combo;
		cv::bitwise_and(mask1, mask2, combo);*/

		cv::Mat mask1;
		col1.threshold(frame, mask1);
		cv::blur(mask1, mask1, cv::Size(5, 5));

		cv::Mat sobel1;
		cv::Sobel(mask1, sobel1, -1, 1, 1);
		cv::dilate(sobel1, sobel1, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3)));

		cv::imshow("borders", sobel1);

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
