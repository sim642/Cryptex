#include "player_module.hpp"

#include <iostream>
#include <boost/asio.hpp>
#include "logger.hpp"

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
#include "ball_targeter.hpp"
#include "goal_targeter.hpp"
#include "border_detector.hpp"
#include "merge_modifier.hpp"

#include "math.hpp"


using namespace std;

const std::map<player_module::state_t, std::string> player_module::state_name =
{
	{Undefined, "Undefined"},
	{Start, "Start"},
	{Manual, "Manual"},
	{BallFind, "BallFind"},
	{BallDrive, "BallDrive"},
	{BallGrab, "BallGrab"},
	{GoalFind, "GoalFind"},
	{GoalAim, "GoalAim"},
	{GoalDrive, "GoalDrive"},
	{AreaEmpty, "AreaEmpty"}
};

player_module::player_module() : state(Undefined)
{

}

player_module::~player_module()
{

}

void player_module::set_state(const state_t &new_state, const string &changer)
{
	if (!changer.empty())
		LOG("player", "state [" + changer + "]:", state_name.at(state), "->", state_name.at(new_state));
	else
		LOG("player", "state:", state_name.at(state), "->", state_name.at(new_state));

	auto it = transitions.find(new_state);
	if (it != transitions.end())
		it->second(state);

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
	ball_targeter::scorer_t scorer = [](const blob &b)
	{
		/*if (b.borderdist < 0.7f)
			return numeric_limits<float>::max();
		else
			return b.dist + fabs(b.angle) / 100;*/

		//if (b.borderdist < 0.7f || b.goaldist < 1.2f)
		if (b.borderdist < 0.f || b.goaldist < 0.2f || b.enemydist < 0.3f)
			return numeric_limits<float>::max();
		else
			return b.dist + fabs(b.angle) / 100.f;
		//return b.borderdist;
	};
	merge_modifier ballmodifier;
	ball_targeter balls(baller, 100, ballmodifier, scorer, 0.2f);

	LOG("player", "waiting team selection button...");
	bool team = m.button(btn_team);
	string team_str = team ? "kollane" : "sinine";
	LOG("player", "attacking", team_str);
	blob_finder goaler(team_str, "goal");
	blob_finder goaler2(team ? "sinine" : "kollane", "goal");
	goal_targeter goals(goaler, goaler2, 50);
	ballmodifier.add_modifier(goals);
	half goalside = half::right;

	blob_finder borderer("valge");
	border_detector borders(borderer, "border");
	ballmodifier.add_modifier(borders);

	cv::namedWindow("Remote");

	set_state(Start, "init");

	transitions[Manual] = [&](state_t prev_state)
	{
		d.stop();
		m.dribbler(0);
	};

	transitions[BallFind] = [&](state_t prev_state)
	{
		m.dribbler(0);
	};

	transitions[BallDrive] = [&](state_t prev_state)
	{
		speed_pid.reset();
		angle_pid.reset();
		rotate_pid.reset();

		//speed_pid.set(100);
		//angle_pid.set(0.5);
		//rotate_pid.set(1.25);
		//angle_pid.set(0.1);
		//rotate_pid.set(1.5);

		/*speed_pid.set(110, 0, 0.2);
		angle_pid.set(0);
		rotate_pid.set(2.5, 0.5, 0.15);*/

		speed_pid.set(90, 0, 0);
		angle_pid.set(0);
		rotate_pid.set(1.5, 0, 0.0);
	};

	transitions[BallGrab] = [&](state_t prev_state)
	{
		balls.reset();
	};

	transitions[GoalAim] = transitions[GoalDrive] = [&](state_t prev_state)
	{
		speed_pid.reset();
		rotate_pid.reset();

		/*speed_pid.set(1.5);
		rotate_pid.set(0.7, 0.5, 0);*/
		/*speed_pid.set(1.75);
		rotate_pid.set(3, 0, 0.3);*/

		//rotate_pid.set(1.0, 0, 2.0);

		/*speed_pid.set(1.5);
		rotate_pid.set(2, 0.7, 0.15);*/

		speed_pid.set(1.5);
		//rotate_pid.set(1.5, 0, 0.22);
		//rotate_pid.set(1.0, 0, 0.05);
		rotate_pid.set(0.3, 0.05, 0.01);
	};

	if (global::coilgun)
		m.charge();
	auto kickit = kicks.begin();

	chrono::high_resolution_clock::time_point framestart;
	while (1)
	{
		framestart = chrono::high_resolution_clock::now();

		m.ping();

		if (global::referee)
		{
			switch (referee.poll()) // only one per cycle
			{
				case referee_controller::None:
					break;

				case referee_controller::Start:
					set_state(BallFind, "referee");
					break;

				case referee_controller::Stop:
					set_state(Manual, "referee");
					d.stop();
					break;
			}
		}

		cv::Mat frame;
		capture >> frame;

		cv::Mat display;
		frame.copyTo(display);

#define SET_STATE(state)	{ \
								set_state(state, "play"); \
								break; \
							}

		switch (state)
		{
			case BallFind:
			case BallDrive:
			{
				/*if (m.ball())
					SET_STATE(GoalFind)*/

				borders.detect(frame);
				goals.update(frame);
				auto ball = balls.update(frame);
				borders.draw(display);
				goals.draw(display);
				balls.draw(display);

				if (state == BallFind && ball)
					SET_STATE(BallDrive)
				else if (state == BallDrive && !ball)
					SET_STATE(BallFind)

				if (state == BallFind)
				{
					if (get_statestart() > 5.f)
						SET_STATE(AreaEmpty)
					else
						d.rotate(max(5.f, 25 - get_statestart() / 1.75f * 10));
				}
				else if (state == BallDrive)
				{
					d.omni(ease_nexpn(get_statestart(), cv::Point2f(1.0, 0.75)) * speed_pid.step(ball->dist), angle_pid.step(ball->angle), rotate_pid.step(ball->angle));

					m.dribbler(ball->dist < 0.8 ? dribblerspeed : 0);
					if (ball->dist < 0.27)
						SET_STATE(BallGrab)
				}

				break;
			}

			case BallGrab:
			{
				d.straight(50);

				if (m.ball())
				{
					this_thread::sleep_for(chrono::milliseconds(250));
					SET_STATE(GoalFind)
				}
				else if (get_statestart() > 0.65f)
					SET_STATE(BallFind)

				break;
			}

			case GoalFind:
			{
				if (!m.ball())
					SET_STATE(BallFind)

				auto goal = goals.update(frame);
				if (goal)
					SET_STATE(GoalAim)
				else
				{
					if (goalside == half::left)
						d.omni(35, -45, 27);
					else
						d.omni(35, 45, -27);
				}

				break;
			}

			case GoalAim:
			{
				/*if (!m.ball())
					SET_STATE(BallFind)*/

				auto goal = goals.update(frame);
				if (goal)
				{
					goals.draw(display);

					auto goalline = goal_targeter::blob2line(*goal, frame.size());
					goalline = lengthen(goalline, 0.05f);

					float goalleftangle = rect2pol(goalline.first).y;
					float goalrightangle = rect2pol(goalline.second).y;

					if (goalleftangle > 0 && goalrightangle < 0)
					{
						blobs_t balls;
						baller.detect_frame(frame, balls);

						bool good = true;
						auto goalpoint = midpoint(goalline);
						for (auto &ball : balls)
						{
							if (dist_lineseg_point({0.f, 0.f}, {0.f, goalpoint.y}, ball.rel) < 0.1f)
							{
								good = false;
								break;
							}
						}

						if (good)
						{
							LOG("player", "shoot");
							//SET_STATE(Manual)

							if (global::coilgun)
							{
								d.stop();

								//this_thread::sleep_for(chrono::milliseconds(300));
								//m.dribbler(0);
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
								SET_STATE(BallFind)
							}
							else
								SET_STATE(GoalDrive)
						}
						else
						{
							LOG("player", "kick tunnel blocked");
							d.omni(60, 45, -2);
						}
					}
					else
						d.omni(speed_pid.step(fabs(goal->angle)), sign(goal->angle) * (-45), rotate_pid.step(goal->angle));
				}
				else
					SET_STATE(GoalFind)

				break;
			}

			case GoalDrive:
			{
				if (!m.ball())
					SET_STATE(BallFind)

				auto goal = goals.update(frame);
				if (goal)
				{
					goals.draw(display);

					d.omni(100, 0, rotate_pid.step(goal->angle));

					if (goal->area > 485.f)
					{
						m.dribbler(0);
						this_thread::sleep_for(chrono::milliseconds(500));
						m.dribbler(dribblerspeed);
						SET_STATE(BallFind)
					}
				}
				else
					SET_STATE(GoalFind)

				break;
			}

			case AreaEmpty:
			{
				borders.detect(frame);
				float dist = borders.dist_closest(cv::Point2f(0, 0)); // my distance
				if (dist > 2.f)
				{
					d.straight(100);
					this_thread::sleep_for(chrono::milliseconds(750));
					SET_STATE(BallFind)
				}
				else
					d.rotate(20);
				break;
			}
		}

		int dt = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - framestart).count();
		float fps = 1000.f / dt;
		cv::putText(display, to_string(fps), cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255));

		imshow("Remote", display);

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
					set_state(BallFind, "manual");
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
