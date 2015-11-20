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
#include "blob_tracker.hpp"

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

	cv::VideoCapture capture(global::video_id);
	if (!capture.isOpened())
		throw runtime_error("capture could not be opened");

	//psmove move;

	srf_dongle srf(io, "/dev/ttyACM0");
	referee_controller referee(srf);

	blob_finder baller("oranz", "ball");
	blob_tracker balltr(50);

	bool team = m.button(btn_team);
	string team_str = team ? "kollane" : "sinine";
	cout << "team: " << team_str << endl;
	blob_finder goaler(team_str, "goal");
	blob_finder goaler2(team ? "sinine" : "kollane", "goal");
	half goalside;

	cv::namedWindow("Remote");

	set_state(Start);

	int ballid = 0;

	if (global::coilgun)
		m.charge();
	auto kickit = kicks.begin();

	chrono::high_resolution_clock::time_point framestart;
	while (1)
	{
		framestart = chrono::high_resolution_clock::now();

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

		cv::Mat framelow;
		cv::resize(frame, framelow, cv::Size(), scalelow, scalelow, CV_INTER_AREA);

		blobs_t gblobs, g2blobs;

		// detect all goal blobs
		goaler.detect_frame(frame, gblobs);
		goaler2.detect_frame(frame, g2blobs);

		// filter robot markings
		blob_finder::angle_filter_out(gblobs, g2blobs, 90, 45);

		auto glarge = blob_finder::largest(gblobs);
		auto g2large = blob_finder::largest(g2blobs);

		if (glarge)
		{
			goalside = glarge->factor >= 0 ? half::left : half::right;
		}
		else if (g2large)
		{
			goalside = g2large->factor >= 0 ? half::right : half::left;
		}

		if (state == Ball)
		{
			m.dribbler(dribblerspeed);

			blobs_t balls;
			baller.detect_frame(frame, balls);
			balltr.update(balls);
			int bestid = balltr.best();

			if (!bestid || !ballid || !balltr[ballid] || (balltr[bestid]->score + 0.2f < balltr[ballid]->score))
				ballid = bestid;

			auto largest = balltr[ballid];
			if (largest) // if ball found
			{
				cv::circle(keyframe, largest->kp.pt, largest->kp.size / 2, cv::Scalar(255, 0, 255), 5);

				d.omni(speed_controller.step(largest->dist), 0, rotate_controller.step(largest->factor));

				if (largest->dist < 0.1)
					set_state(BallGrab);
			}
			else
				d.rotate(max(5.f, 25 - get_statestart() / 1.5f * 10));
		}
		else if (state == BallGrab)
		{
			balltr.clear();

			d.straight(50);

			if (m.ball())
			{
				this_thread::sleep_for(chrono::milliseconds(500));
				set_state(GoalFind);
			}
			else if (get_statestart() > 2.f)
				set_state(Ball);
		}
		else if (state == GoalFind || state == Goal)
		{
			if (glarge) // if goal found
			{
				cv::circle(keyframe, glarge->kp.pt / scalelow, glarge->kp.size / 2 / scalelow, cv::Scalar(255, 0, 255), 5);

				if (state == GoalFind)
				{
					if (abs(glarge->factor) < 0.1)
					{
						blobs_t balls;
						baller.detect_frame(frame, balls);

						bool good = true;
						for (auto &ball : balls)
						{
							if (abs(ball.factor) < 0.1)
							{
								good = false;
								break;
							}
						}

						if (good)
						{
							if (global::coilgun)
							{
								d.stop();
								this_thread::sleep_for(chrono::milliseconds(500));
								m.dribbler(0);
								this_thread::sleep_for(chrono::milliseconds(250));
								m.kick(*kickit);
								kickit++;
								this_thread::sleep_for(chrono::milliseconds(100));

								if (kickit == kicks.end())
								{
									m.charge();
									kickit = kicks.begin();
								}

								m.dribbler(dribblerspeed);
								set_state(Ball);
							}
							else
								set_state(Goal);
						}
						else
						{
							cout << "kick block" << endl;
							d.omni(50, 45, -2);
							//this_thread::sleep_for(chrono::milliseconds(100)); // TODO: smooth driving, not time
							set_state(GoalFind);
						}
					}
					else
						d.omni(speed_controller.step(fabs(glarge->factor)), sign(glarge->factor) * (-90), rotate_controller.step(glarge->factor));
				}
				else
				{
					d.omni(100, 0, rotate_controller.step(glarge->factor));

					if (glarge->kp.size > 485.f * pow(scalelow, 2))
					{
						m.dribbler(0);
						this_thread::sleep_for(chrono::milliseconds(500));
						m.dribbler(dribblerspeed);
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
				}
				else
					d.straight(30);
			}
		}

		int dt = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - framestart).count();

		imshow("Remote", keyframe);

		char key = cv::waitKey(max(1, 1000 / 60 - dt));
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

			case '1':
				set_state(Start);
				break;

			case '2':
				set_state(Manual);
				break;

			case '3':
				set_state(Ball);
				break;

			case '4':
				set_state(BallGrab);
				break;

			case '5':
				set_state(GoalFind);
				break;

			case '6':
				set_state(Goal);
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
