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
	cv::VideoCapture capture(0);

	calibrator_window calibrator(capture);
	calibrator.calibrate("red", "cap");

	cv::namedWindow("tracking");

	blob_finder finder("red", "cap");
	blob_tracker tracker(100);

	cv::VideoWriter writer("tracking.avi",
							//CV_FOURCC('M', 'J', 'P', 'G'),
							CV_FOURCC('P', 'I', 'M', '1'),
							30,
							cv::Size(capture.get(CV_CAP_PROP_FRAME_WIDTH),
										capture.get(CV_CAP_PROP_FRAME_HEIGHT)),
							true);


	while (1)
	{
		auto framestart = chrono::high_resolution_clock::now();
		cv::Mat frame;
		capture >> frame;

		cv::Mat display;
		frame.copyTo(display);

		blobs_t blobs;
		finder.detect_frame(frame, blobs);
		tracker.update(blobs);

		for (auto &p : tracker.get_all())
		{
			int id = p.first;
			const blob &b = p.second;

			cv::circle(display, b.kp.pt, b.kp.size / 2, cv::Scalar(0, 255, 0), 2);
			cv::putText(display, to_string(id), b.kp.pt, cv::FONT_HERSHEY_PLAIN, 1.5, cv::Scalar(0, 0, 255), 2);
		}

		imshow("tracking", display);
		writer << display;

		int dt = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - framestart).count();
		char key = cv::waitKey(max(1, 1000 / 30 - dt));
		switch (key)
		{
			case 'q':
				return module::type::menu;
		}
	}

	return module::type::menu;
}
