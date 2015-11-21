#include "test_module.hpp"

#include <iostream>
#include <boost/asio.hpp>
#include <chrono>
#include <thread>

#include "global.hpp"
#include "device_id.hpp"

#include "rs485_dongle.hpp"
#include "driver.hpp"
#include "main_controller.hpp"

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
	global::env = "test";
	cv::VideoCapture capture(1);

	cv::namedWindow("border");
	cv::namedWindow("edge");
	int lower = 0, upper = 255;
	cv::createTrackbar("lower", "border", &lower, 512);
	cv::createTrackbar("upper", "border", &upper, 512);

	while (1)
	{
		auto framestart = chrono::high_resolution_clock::now();
		cv::Mat frame;
		capture >> frame;

		cv::Mat gray;
		cv::cvtColor(frame, gray, CV_BGR2GRAY);

		cv::blur(gray, gray, cv::Size(3, 3));

		cv::Mat canny;
		cv::Canny(gray, canny, lower, upper);

		cv::Mat display;
		//cv::cvtColor(canny, display, CV_GRAY2BGR);
		frame.copyTo(display);

		vector<cv::Vec4i> lines;
		cv::HoughLinesP(canny, lines, 1, CV_PI/180, 75, 100, 10);
		for( size_t i = 0; i < lines.size(); i++ )
		{
			cv::Vec4i l = lines[i];
			cv::line(display, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(0,0,255), 3, CV_AA);
		}

		cv::imshow("border", display);
		cv::imshow("edge", canny);

		int dt = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - framestart).count();
		char key = cv::waitKey(max(1, 1000 / 60 - dt));
		switch (key)
		{
			case 'q':
				return module::type::menu;
		}
	}

	return module::type::menu;
}
