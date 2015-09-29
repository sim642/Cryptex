#include "psmove_module.hpp"

#include <iostream>
#include <stdexcept>
#include <psmoveapi/psmove.h>

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
    PSMove *move;

    if (!psmove_init(PSMOVE_CURRENT_VERSION))
		throw runtime_error("PSMove API init failed");

	cout << "PSMove count: " << psmove_count_connected() << endl;

	move = psmove_connect();
	if (move == NULL)
		throw runtime_error("PSMove controller not found");

	//psmove_set_rate_limiting(move, PSMove_True);

	boost::asio::io_service io;
	rs485_dongle dongle(io, "/dev/ttyUSB0");
	driver d(dongle);

	psmove_enable_orientation(move, PSMove_True);
	if (!psmove_has_orientation(move))
		throw runtime_error("PSMove no orientation");

	int prev_seq = 0;
	while (1)
	{
		int seq = psmove_poll(move);
		if ((prev_seq > 0) && ((prev_seq % 16) != (seq - 1)))
			continue;

		float w, x, y, z;
		psmove_get_orientation(move, &w, &x, &y, &z);

		/*float phi = atan2(y*z+w*x, 0.5f-(x*x+y*y));
		float the = asin(-2*(x*z-w*y));
		float psi = atan2(x*y+w*z, 0.5f-(y*y+z*z));*/
		float phi = atan2(x*z+y*w, x*w-y*z);
		float the = acos(-x*x-y*y+z*z+w*w);
		float psi = atan2(x*z-y*w, y*z+x*w);
		//cout << w << "\t" << x << "\t" << y << "\t" << z << endl;
		cout << phi << "\t" << the << "\t" << psi << endl;

		//d.rotate(psi * 25);
		d.omni(psmove_get_trigger(move) * 0.3, rad2deg(phi), psi * 25);

		/*int t = psmove_get_trigger(move);
		psmove_set_leds(move, 0, t, 0);
		psmove_set_rumble(move, t);
		psmove_update_leds(move);

		cout << t << endl;
		d.omni(t * 0.5, 0);*/

		this_thread::sleep_for(chrono::milliseconds(1000 / 60));

		prev_seq = seq;
	}

	psmove_disconnect(move);

	return module::type::menu;
}
