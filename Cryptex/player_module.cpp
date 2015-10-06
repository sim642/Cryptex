#include "player_module.hpp"

#include <iostream>

#include <boost/asio.hpp>
#include "rs485_dongle.hpp"
#include "driver.hpp"

#include "psmove.hpp"

#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include "blob_finder.hpp"

#include "pid_controller.hpp"
#include "math.hpp"

#include "global.hpp"

using namespace std;

player_module::player_module() : state(Start)
{

}

player_module::~player_module()
{

}

module::type player_module::run(const module::type &prev_module)
{
	boost::asio::io_service io;
	rs485_dongle dongle(io, "/dev/ttyUSB0");

	driver d(dongle);

	cv::VideoCapture capture(global::video_id);
	if (!capture.isOpened())
		throw runtime_error("capture could not be opened");

	if (!psmove_init(PSMOVE_CURRENT_VERSION))
		throw runtime_error("PSMove API init failed");

	psmove move;

	blob_finder baller("oranz", "ball");
	blob_finder goaler("kollane", "goal");

	pid_controller speed_controller, rotate_controller;

	cv::namedWindow("Remote");
	while (1)
	{
		cv::Mat frame;
		capture >> frame;

		cv::Mat keyframe;
		frame.copyTo(keyframe);

		if (state == Ball)
		{
			auto largest = baller.largest(frame);
			if (largest.size >= 0.f) // if blob found
			{
				cv::circle(keyframe, largest.pt, largest.size / 2, cv::Scalar(255, 0, 255), 5);

				int diff = frame.cols / 2 - largest.pt.x;
				float factor = float(diff) / (frame.cols / 2);

				float dist = (frame.rows - largest.pt.y) / float(frame.rows);
				d.omni(speed_controller.step(dist), 0, rotate_controller.step(factor));

				if (dist < 0.1)
				{
					state = GoalFind;
					speed_controller.reset();
					rotate_controller.reset();
					speed_controller.Kp = 17;
					rotate_controller.Kp = 13;
				}
			}
			else
				d.rotate(20);
		}
		else if (state == GoalFind || state == Goal)
		{
			auto largest = goaler.largest(frame);
			if (largest.size >= 0.f) // if blob found
			{
				cv::circle(keyframe, largest.pt, largest.size / 2, cv::Scalar(255, 0, 255), 5);

				int diff = frame.cols / 2 - largest.pt.x;
				float factor = float(diff) / (frame.cols / 2);

				float dist = (frame.rows - largest.pt.y) / float(frame.rows);

				if (state == GoalFind)
				{
					if (abs(factor) < 0.2)
						state = Goal;
					else
						d.omni(speed_controller.step(fabs(factor)), sign(factor) * (-90), rotate_controller.step(factor));
				}
				else
				{
					d.omni(40, 0, rotate_controller.step(factor));
				}
			}
			else
			{
				if (state == GoalFind)
					d.omni(17, -90, 13);
				else
					d.straight(30);
			}
		}

		imshow("Remote", keyframe);

		char key = cv::waitKey(1000 / 60);
		switch (key)
		{
			case 27:
			case 'q':
				return module::type::menu;

			case 'w':
				d.straight(75);
				break;

			case 's':
				d.straight(-50);
				break;

			case 'a':
				d.rotate(30);
				break;

			case 'd':
				d.rotate(-30);
				break;

			case ' ':
				d.stop();
				break;

			case 'e':
				if (state == Manual)
				{
					state = Ball;
					speed_controller.reset();
					rotate_controller.reset();
					speed_controller.Kp = 80;
					rotate_controller.Kp = 30;
				}
				else
					state = Manual;

				break;
		}

		bool polled = move.poll();
		if (!polled)
			continue;

		if (state == Start && move.pressed(Btn_START))
			state = Manual;

		if (move.pressed(Btn_MOVE))
		{
			state = Manual;
			d.stop();
		}
	}
}
