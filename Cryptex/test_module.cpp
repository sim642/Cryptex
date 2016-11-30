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

#include "mbed_main_controller.hpp"
#include "mbed_driver.hpp"

using namespace std;

test_module::test_module()
{

}

test_module::~test_module()
{

}

module::type test_module::run(const module::type &prev_module)
{
	{
		load_multi_camera();
	}
	cout << "asd" << endl;
	for (int i = 0; i < 10; i++)
	{
		//cv::VideoCapture capture(0);
		cv::VideoCapture capture("/dev/video0", cv::CAP_V4L);
		cout << capture.isOpened() << flush;

		//cv::VideoCapture capture2(1);
		//cv::VideoCapture capture2("/dev/video1", cv::CAP_V4L);
		//cout << capture2.isOpened() << endl;
	}
	/*cv::VideoCapture capture("./pics/seq%02d.jpg", cv::CAP_IMAGES);
	//cv::VideoCapture capture("/dev/video8", cv::CAP_V4L2);
	cout << capture.isOpened() << endl;
	cv::namedWindow("test");

	cout << capture.get(cv::CAP_PROP_POS_FRAMES) << endl;
	cout << capture.get(cv::CAP_PROP_FRAME_COUNT) << endl;

	while (1)
	{
		capture.set(cv::CAP_PROP_POS_FRAMES, 0);

		cv::Mat frame;
		capture >> frame;

		cv::imshow("test", frame);
		char key = cv::waitKey(1000 / 30);
		switch (key)
		{
			case 'q':
			{
				return module::type::menu;
			}
		}
	}*/

	/*boost::asio::io_service io;
	mbed_main_controller m(io, "/dev/ttyACM25");
	mbed_driver d(m);

	m.charge();

	while (1)
	{
		d.omni(50, 0);
		//d.rotate(50);
		this_thread::sleep_for(chrono::milliseconds(100));
	}*/

	return module::type::menu;
}
