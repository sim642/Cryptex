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

		case BallGrab:
			cout << "BallGrab";
			break;

		case GoalFind:
			cout << "GoalFind";
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
	m.dribbler(255);

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
	blob_finder goaler2(team ? "sinine" : "kollane", "goal");
	half goalside;

	cv::namedWindow("Remote");

	set_state(Start);

	m.charge();
	int kickcnt = 0;

	while (1)
	{
		m.ping();

		switch (referee.poll()) // only one per cycle
		{
			case referee_controller::None:
				break;

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


		auto glarge = goaler.largest(frame);
		auto g2large = goaler2.largest(frame);

		if (glarge.size > 0.f)
		{
			goalside = get<0>(*goaler.factordist(frame, glarge)) >= 0 ? half::left : half::right;
		}
		else if (g2large.size > 0.f)
		{
			goalside = get<0>(*goaler2.factordist(frame, g2large)) >= 0 ? half::right : half::left;
		}

		//cout << "goalside: " << (goalside == half::left ? "left" : "right") << endl;

		if (state == Ball)
		{
			auto largest = baller.largest(frame);
			auto factordist = baller.factordist(frame, largest);

			if (factordist) // if blob found
			{
				float factor, dist;
				tie(factor, dist) = *factordist;

				cv::circle(keyframe, largest.pt, largest.size / 2, cv::Scalar(255, 0, 255), 5);

				d.omni(speed_controller.step(dist), 0, rotate_controller.step(factor));

				if (dist < 0.1)
					set_state(BallGrab);
			}
			else
				d.rotate(max(5.f, 25 - get_statestart() / 2 * 10));
		}
		else if (state == BallGrab)
		{
			d.straight(50);

			if (m.ball())
				set_state(GoalFind);
		}
		//else if (state == GoalFind || state == Goal)
		else if (state == GoalFind)
		{
			auto &largest = glarge;
			auto factordist = goaler.factordist(frame, largest);

			cout << largest.size << endl;
			if (factordist) // if blob found
			{
				float factor, dist;
				tie(factor, dist) = *factordist;

				cv::circle(keyframe, largest.pt, largest.size / 2, cv::Scalar(255, 0, 255), 5);

				if (state == GoalFind)
				{
					if (abs(factor) < 0.1)
					{
						cv::Mat mask;
						baller.threshold(frame, mask);

						vector<cv::KeyPoint> keypoints;
						baller.detect(mask, keypoints);

						bool good = true;
						for (auto &kp : keypoints)
						{
							float factor, dist;
							tie(factor, dist) = *baller.factordist(frame, kp);

							if (abs(factor) < 0.1)
							{
								good = false;
								break;
							}
						}

						if (good)
						{
							//set_state(Goal);
							d.stop();
							this_thread::sleep_for(chrono::milliseconds(500));
							m.dribbler(0);
							this_thread::sleep_for(chrono::milliseconds(250));
							m.kick();
							kickcnt++;
							this_thread::sleep_for(chrono::milliseconds(100));

							if (kickcnt % 2 == 0)
								m.charge();

							m.dribbler(255);
							//set_state(Ball);
							set_state(Manual);

							//throw;
						}
						else
						{
							cout << "kick block" << endl;
							d.omni(50, 60, 5);
							this_thread::sleep_for(chrono::milliseconds(100)); // TODO: smooth driving, not time
							set_state(GoalFind);
						}
					}
					else
						d.omni(speed_controller.step(fabs(factor)), sign(factor) * (-90), rotate_controller.step(factor));
						//d.rotate(speed_controller.step(fabs(factor)));
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
				{
					cout << "goalside: " << (goalside == half::left ? "left" : "right") << endl;
					if (goalside == half::left)
						d.omni(17, -90, 13);
					else
						d.omni(17, 90, -13);
					//d.rotate(17);
				}
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

			case 'c':
				m.charge();
				break;

			case 'k':
				m.kick();
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
