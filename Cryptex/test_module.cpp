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

	blob_finder baller("oranz", "ball");
	blob_finder blobber("valge");
	blobber.opening = true;
	border_detector borders(blobber);

	int i = 0;
	while (1)
	{
		cv::Mat frame;
		images[i].copyTo(frame);

		cv::Mat display;
		frame.copyTo(display);

		blobs_t balls;
		baller.detect_frame(frame, balls);

		lines_t lines;
		borders.detect(frame, lines);

		cout << border_detector::dist_closest(lines, cv::Point2f(0, 0)) << endl;

		for (auto &ball : balls)
		{
			cout << border_detector::dist_closest(lines, ball.rel) << "\t";
		}
		cout << endl;


		borders.draw(display);

		cv::imshow("walls", display);

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
