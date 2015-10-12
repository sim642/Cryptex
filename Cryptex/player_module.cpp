#include "player_module.hpp"

#include <iostream>

#include <boost/asio.hpp>
#include "rs485_dongle.hpp"
#include "driver.hpp"

#include "psmove.hpp"
#include "srf_dongle.hpp"

#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include "blob_finder.hpp"

#include "math.hpp"

#include "global.hpp"

using namespace std;

player_module::player_module() : state(Start)
{

}

player_module::~player_module()
{

}

void player_module::set_state(const state_t &new_state)
{
	cout << "state: ";
	switch (new_state)
	{
		case Start:
			cout << "Start";
			break;

		case Manual:
			cout << "Manual";
			break;

		case Ball:
			cout << "Ball";
			speed_controller.reset();
			rotate_controller.reset();
			speed_controller.Kp = 80;
			rotate_controller.Kp = 30;
			break;

		case GoalFind:
			cout << "BallFind";
			speed_controller.reset();
			rotate_controller.reset();
			speed_controller.Kp = 17;
			rotate_controller.Kp = 13;
			break;

		case Goal:
			cout << "Goal";
			break;
	}
	cout << endl;

	state = new_state;
}

module::type player_module::run(const module::type &prev_module)
{
	boost::asio::io_service io;

	rs485_dongle dongle(io, "/dev/ttyUSB0");

	driver d(dongle);

	cv::VideoCapture capture(global::video_id);
	if (!capture.isOpened())
		throw runtime_error("capture could not be opened");

	/*if (!psmove_init(PSMOVE_CURRENT_VERSION))
		throw runtime_error("PSMove API init failed");

	psmove move;*/

	srf_dongle srf(io, "/dev/ttyACM0");

	blob_finder baller("oranz", "ball");
	blob_finder goaler("kollane", "goal");

	cv::namedWindow("Remote");
	while (1)
	{
		auto srf_data = srf.recv_parsed();
		if (get<0>(srf_data))
		{
			char field, target;
			string cmd;
			tie(field, target, cmd) = srf_data;

			if (field == global::field && (target == global::id || target == 'X'))
			{
				if (cmd == "START")
				{
					set_state(Ball);
				}
				else if (cmd == "STOP")
				{
					set_state(Manual);
					d.stop();
				}

				if (target == global::id)
				{
					srf.send(field, target, "ACK");
				}
			}
		}

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
					set_state(GoalFind);
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
						set_state(Goal);
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
					set_state(Ball);
				else
				{
					set_state(Manual);
					d.stop();
				}

				break;
		}

		/*bool polled = move.poll();
		if (!polled)
			continue;

		if (state == Start && move.pressed(Btn_START))
			set_state(Manual);

		if (move.pressed(Btn_MOVE))
			set_state(Manual);*/
	}
}
