#include "psmove_module.hpp"

#include <iostream>
#include <stdexcept>
#include "logger.hpp"
#include "psmove.hpp"

#include <boost/asio.hpp>
#include "rs485_dongle.hpp"
#include "serial_controller.hpp"
#include "driver.hpp"
#include "main_controller.hpp"

#include <cmath>
#include "math.hpp"

#include "global.hpp"
#include <opencv2/opencv.hpp>

#include <chrono>
#include <thread>

using namespace std;

psmove_module::psmove_module()
{

}

psmove_module::~psmove_module()
{

}

module::type psmove_module::run(const module::type &prev_module)
{
	cv::VideoCapture capture(global::video_id);

	LOG("psmove", "connected count:", psmove::connected_count());

	psmove move;

	LOG("psmove", "battery:", move.battery());

	//psmove_set_rate_limiting(move, PSMove_True);

	boost::asio::io_service io;
	rs485_dongle dongle(io, "/dev/ttyUSB0");
	serial_controller mcontrol(io, "/dev/ttyACM0");
	driver d(dongle);
	main_controller m(&mcontrol);

	if (!move.orientation(true))
		throw runtime_error("PSMove no orientation");

	bool running = false;
	bool dribbler = false;

	cv::namedWindow("vision");

	m.charge();

	while (1)
	{
		//cv::Mat frame;
		//capture >> frame;

		//cv::imshow("vision", frame);

		bool polled = move.poll();
		m.ping();

		if (move.pressed(Btn_START))
			running = true;

		if (move.pressed(Btn_MOVE))
			return module::type::menu;

		if (move.pressed(Btn_SQUARE))
		{
			if (dribbler)
				m.dribbler(0);
			else
				m.dribbler(255);

			dribbler = !dribbler;
		}

		if (move.pressed(Btn_CROSS))
		{
			m.dribbler(0);
			this_thread::sleep_for(chrono::milliseconds(100));
			m.kick(3000);
			m.charge();
			cout << "kick" << endl;
		}


		if (!polled)
			continue;

		auto orientation = move.orientation();

		//cout << orientation[0] << "\t" << orientation[1] << "\t" << orientation[2] << endl;

		move.leds(cv::Scalar_<int>(running * 255, 0, 0));

		if (running)
			d.omni(move.trigger() * 0.3, rad2deg(orientation[0]), orientation[2] * 15);

		this_thread::sleep_for(chrono::milliseconds(1000 / 60));
	}

	return module::type::menu;
}
