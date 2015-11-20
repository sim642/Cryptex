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

void player_module::set_state(const state_t &new_state, const string &changer)
{
	cout << "state";
	if (!changer.empty())
		cout << " [" << changer << "]";
	cout << ": ";
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
			speed_controller.Kp = 115;
			rotate_controller.Kp = 45;
			speed_controller.Ki = 0;
			rotate_controller.Ki = 0;
			//speed_controller.Kd = 0;
			//rotate_controller.Kd = 0;
			break;

		case BallGrab:
			cout << "BallGrab";
			break;

		case GoalFind:
			cout << "GoalFind";
			speed_controller.reset();
			rotate_controller.reset();
			speed_controller.Kp = 25;
			rotate_controller.Kp = 25;
			speed_controller.Ki = 0;
			rotate_controller.Ki = 0;
			//speed_controller.Kd = rotate_controller.Kd = 0;
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
	blob_finder goaler2(team ? "sinine" : "kollane", "goal");
	half goalside;

	cv::namedWindow("Remote");

	set_state(Start, "init");

	if (global::coilgun)
		m.charge();
	auto kickit = kicks.begin();

	chrono::high_resolution_clock::time_point framestart;
	long long framecnt = 0;

	while (1)
	{
		framestart = chrono::high_resolution_clock::now();

		m.ping();

		switch (referee.poll()) // only one per cycle
		{
			case referee_controller::None:
				break;

			case referee_controller::Start:
				set_state(Ball, "referee");
				break;

			case referee_controller::Stop:
				set_state(Manual, "referee");
				d.stop();
				break;
		}

		cv::Mat frame;
		capture >> frame;
		framecnt++;

		cv::Mat keyframe;
		frame.copyTo(keyframe);

		cv::Mat framelow;
		cv::resize(frame, framelow, cv::Size(), scalelow, scalelow, CV_INTER_AREA);

		blob_finder::keypoints_t gpoints, g2points;

		//if ((state == GoalFind || state == Goal) || framecnt % 10 == 0)
		if ((state == GoalFind || state == Goal))
		{
			// detect all goal blobs
			{
				cv::Mat mask;

				goaler.threshold(framelow, mask);
				goaler.detect(mask, gpoints);

				goaler2.threshold(framelow, mask);
				goaler2.detect(mask, g2points);
			}

			cv::drawKeypoints(keyframe, gpoints, keyframe, cv::Scalar(255, 0, 255), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
			cv::drawKeypoints(keyframe, g2points, keyframe, cv::Scalar(255, 0, 255), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
			//cout << "B: " << gpoints.size() << " " << g2points.size() << endl;

			// filter robot markings
			blob_finder::angle_filter_out(gpoints, g2points, 90, 70);

			cv::drawKeypoints(keyframe, gpoints, keyframe, cv::Scalar(255, 255, 0), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
			cv::drawKeypoints(keyframe, g2points, keyframe, cv::Scalar(255, 255, 0), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
			//cout << "A: " << gpoints.size() << " " << g2points.size() << endl;
		}

		auto glarge = blob_finder::largest(gpoints);
		auto g2large = blob_finder::largest(g2points);

		if (glarge.size > 0.f)
		{
			goalside = get<0>(*goaler.factordist(framelow, glarge)) >= 0 ? half::left : half::right;
		}
		else if (g2large.size > 0.f)
		{
			goalside = get<0>(*goaler2.factordist(framelow, g2large)) >= 0 ? half::right : half::left;
		}

		//cout << "goalside: " << (goalside == half::left ? "left" : "right") << endl;

		if (state == Ball)
		{
			cout << "." << flush;
			if (m.ball())
				set_state(GoalFind, "play");

			//m.dribbler(0);
			m.dribbler(dribblerspeed);

			auto largest = baller.largest(frame);
			auto factordist = baller.factordist(frame, largest);

			if (factordist) // if blob found
			{
				float factor, dist;
				tie(factor, dist) = *factordist;

				cv::circle(keyframe, largest.pt, largest.size / 2, cv::Scalar(255, 0, 255), 5);

				d.omni(speed_controller.step(dist), 0, rotate_controller.step(factor));

				if (dist < 0.3)
					set_state(BallGrab, "play");
			}
			else
				d.rotate(max(10.f, 35 - get_statestart() / 2.f * 10));
		}
		else if (state == BallGrab)
		{
			//m.dribbler(dribblerspeed);
			d.straight(50);
			cout << "bg" << endl;

			if (m.ball())
			{
				this_thread::sleep_for(chrono::milliseconds(250));
				set_state(GoalFind, "play");
			}
			else if (get_statestart() > 0.65f)
				set_state(Ball, "play");

			cout << "bg2" << endl;
		}
		else if (state == GoalFind || state == Goal)
		{
			if (!m.ball())
				set_state(Ball, "play");

			auto &largest = glarge;
			auto factordist = goaler.factordist(framelow, largest);

			cout << largest.size / scalelow << endl;
			if (factordist) // if blob found
			{
				float factor, dist;
				tie(factor, dist) = *factordist;

				cv::circle(keyframe, largest.pt / scalelow, largest.size / 2 / scalelow, cv::Scalar(255, 0, 255), 5);

				if (state == GoalFind)
				{
					if (abs(factor) < max(0.15f, (1 - dist) / 3))
					//if (abs(factor) < 0.15f)
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
							if (global::coilgun)
							{
								d.stop();
								this_thread::sleep_for(chrono::milliseconds(300));
								m.dribbler(0);
								this_thread::sleep_for(chrono::milliseconds(150));
								m.kick(*kickit);
								kickit++;
								this_thread::sleep_for(chrono::milliseconds(50));

								if (kickit == kicks.end())
								{
									m.charge();
									kickit = kicks.begin();
								}

								m.dribbler(dribblerspeed);
								set_state(Ball, "play");
							}
							else
								set_state(Goal, "play");
						}
						else
						{
							cout << "kick block" << endl;
							d.omni(50, 45, -2);
							//this_thread::sleep_for(chrono::milliseconds(100)); // TODO: smooth driving, not time
							set_state(GoalFind, "play");
						}
					}
					else
						d.omni(speed_controller.step(fabs(factor)), sign(factor) * (-90), rotate_controller.step(factor));
						//d.rotate(speed_controller.step(fabs(factor)));
				}
				else
				{
					d.omni(100, 0, rotate_controller.step(factor));

					if (largest.size > 485.f * pow(scalelow, 2))
					{
						m.dribbler(0);
						this_thread::sleep_for(chrono::milliseconds(500));
						m.dribbler(dribblerspeed);
						set_state(Ball, "play");
					}
				}
			}
			else
			{
				if (state == GoalFind)
				{
					cout << "goalside: " << (goalside == half::left ? "left" : "right") << endl;
					if (goalside == half::left)
						d.omni(35, -45, 30);
					else
						d.omni(35, 45, -30);
					//d.rotate(17);
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
					set_state(Ball, "manual");
				}
				else
				{
					set_state(Manual, "manual");
					d.stop();
				}

				break;

			case 'c':
				m.charge();
				break;

			case 'k':
				m.kick();
				break;

			/*case '1':
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
				break;*/
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
