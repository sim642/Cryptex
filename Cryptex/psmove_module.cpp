#include "psmove_module.hpp"

#include <iostream>
#include <stdexcept>
#include "psmove.hpp"

#include <boost/asio.hpp>
#include "rs485_dongle.hpp"
#include "driver.hpp"

#include <cmath>
#include "math.hpp"

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
	psmove::init();

	cout << "PSMove count: " << psmove::connected_count() << endl;

	psmove move;

	cout << "PSMove battery: " << move.battery() << endl;

	//psmove_set_rate_limiting(move, PSMove_True);

	boost::asio::io_service io;
	rs485_dongle dongle(io, "/dev/ttyUSB0");
	driver d(dongle);

	if (!move.orientation(true))
		throw runtime_error("PSMove no orientation");

	bool running = false;

	while (1)
	{
		bool polled = move.poll();

		if (move.pressed(Btn_START))
			running = true;

		if (move.pressed(Btn_MOVE))
			return module::type::menu;


		if (!polled)
			continue;

		auto orientation = move.orientation();

		cout << orientation[0] << "\t" << orientation[1] << "\t" << orientation[2] << endl;

		move.leds(cv::Scalar_<int>(running * 255, 0, 0));

		if (running)
			d.omni(move.trigger() * 0.3, rad2deg(orientation[0]), orientation[2] * 15);

		this_thread::sleep_for(chrono::milliseconds(1000 / 60));
	}

	return module::type::menu;
}
