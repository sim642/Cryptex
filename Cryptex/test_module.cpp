#include "test_module.hpp"

#include <iostream>
#include <boost/asio.hpp>
#include "serial_scanner.hpp"
#include "rs485_dongle.hpp"
#include "motor.hpp"
#include <chrono>
#include <thread>

#include "global.hpp"

#include "driver.hpp"
#include "blob_finder.hpp"
#include "pid_controller.hpp"

using namespace std;

test_module::test_module()
{

}

test_module::~test_module()
{

}

module::type test_module::run(const module::type &prev_module)
{
	boost::asio::io_service io;

	rs485_dongle dongle(io, "/dev/ttyUSB0");
	driver d(dongle);

	cv::VideoCapture capture(global::video_id);
	if (!capture.isOpened())
		throw runtime_error("capture could not be opened");

	blob_finder blobber("kollane", "goal");

	/*for (int i = 0; i < 20; i++)
	{
		d.omni(17, -90, 13);
		this_thread::sleep_for(chrono::milliseconds(150));
	}*/

	cv::namedWindow("Remote");
	while (1)
	{
		cv::Mat frame;
		capture >> frame;

		cv::Mat keyframe;
		frame.copyTo(keyframe);


		auto largest = blobber.largest(frame);
		if (largest.size >= 0.f) // if blob found
		{
			cv::circle(keyframe, largest.pt, largest.size / 2, cv::Scalar(255, 0, 255), 5);

			int diff = frame.cols / 2 - largest.pt.x;
			float factor = float(diff) / (frame.cols / 2);

			float dist = (frame.rows - largest.pt.y) / float(frame.rows);
			//d.omni(speed_controller.step(dist), 0, rotate_controller.step(factor));

			cout << factor << endl;

			if (abs(factor) < 0.2)
				d.stop();
			if (factor > 0)
				d.omni(17 * fabs(factor), -90, 13 * fabs(factor));
			else
				d.omni(17 * fabs(factor), 90, -13 * fabs(factor));
		}

		imshow("Remote", keyframe);

		char key = cv::waitKey(1000 / 60);
		switch (key)
		{
			case 27:
			case 'q':
				return module::type::menu;
		}
	}

	/*for (int i = 0; i < 4; i++)
	{
		d.omni(50, i * 90);
		this_thread::sleep_for(chrono::seconds(1));
	}*/

	//device_controller *control = dongle[1];
	/*serial_scanner dongle(io, "ttyACM");
	device_controller *control = dongle[1];*/
	/*motor mot(control);
	while (1)
	{
		cout << mot.encoder() << endl;
		this_thread::sleep_for(chrono::milliseconds(2));
	}*/

	return module::type::menu;
}
