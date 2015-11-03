#include "player_module.hpp"

#include <iostream>
#include <boost/asio.hpp>

#include "rs485_dongle.hpp"
#include "global.hpp"
#include "driver.hpp"
#include "main_controller.hpp"

#include "psmove.hpp"
#include "srf_dongle.hpp"
#include "referee_controller.hpp"

#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include "blob_finder.hpp"

#include "math.hpp"


using namespace std;

player_module::player_module() : state(Undefined)
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

		default:
			throw domain_error("player going to invalid state");
	}
	cout << endl;

	state = new_state;
	reset_statestart();
}

module::type player_module::run(const module::type &prev_module)
{
	boost::asio::io_service io;

	rs485_dongle dongle(io, "/dev/ttyUSB0");

	driver d(dongle);
	main_controller m(dongle[device_id::main]);

	cv::VideoCapture capture(global::video_id);
	if (!capture.isOpened())
		throw runtime_error("capture could not be opened");

	//psmove move;

	srf_dongle srf(io, "/dev/ttyACM0");
	referee_controller referee(srf);

	blob_finder baller("oranz", "ball");

	bool team = m.button(btn_team);
	string team_str = team ? "kollane" : "sinine";
	cout << "team: " << team_str << endl;
	blob_finder goaler(team_str, "goal");

	cv::namedWindow("Remote");

	set_state(Start);
	while (1)
	{
		switch (referee.poll()) // only one per cycle
		{
			case referee_controller::Start:
				set_state(Ball);
				break;

			case referee_controller::Stop:
				set_state(Manual);
				d.stop();
				break;
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
				d.rotate(max(5.f, 25 - get_statestart() / 2 * 10));
		}
		else if (state == GoalFind || state == Goal)
		{
			auto largest = goaler.largest(frame);
			cout << largest.size << endl;
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
					d.omni(100, 0, rotate_controller.step(factor));

					if (largest.size > 485.f)
					{
						set_state(Ball);
					}
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
					set_state(Ball);
				}
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

void player_module::reset_statestart()
{
	statestart = chrono::high_resolution_clock::now();
}

float player_module::get_statestart()
{
	return chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - statestart).count() / 1000.f;
}
