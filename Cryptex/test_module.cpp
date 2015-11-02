#include "test_module.hpp"

#include <iostream>
#include <boost/asio.hpp>
#include <chrono>
#include <thread>

#include "global.hpp"
#include "device_id.hpp"

#include "rs485_dongle.hpp"
#include "main_controller.hpp"

using namespace std;

test_module::test_module()
{

}

test_module::~test_module()
{

}

module::type test_module::run(const module::type &prev_module)
{
	boost::asio::io_service io;

	rs485_dongle dongle(io, "/dev/ttyUSB0");
	main_controller mainer(dongle[device_id::main]);

	while (1)
	{
		mainer.ping(); // keep failsafe from triggering

		cout << mainer.ball() << endl;

		this_thread::sleep_for(chrono::milliseconds(100));
	}

	return module::type::menu;
}
