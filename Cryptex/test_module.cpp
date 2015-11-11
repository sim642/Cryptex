#include "test_module.hpp"

#include <iostream>
#include <boost/asio.hpp>
#include <chrono>
#include <thread>

#include "global.hpp"
#include "device_id.hpp"

#include "rs485_dongle.hpp"
#include "driver.hpp"
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
	driver d(dongle);
	main_controller m(dongle[device_id::main]);

	//m.dribbler(255);
	//this_thread::sleep_for(chrono::seconds(1));

	while (1)
	{
		//m.ping();
		d.omni(35, -85, 25);
		//d.omni(45, -85, 32);
		//d.omni(45, -80, 29);
		this_thread::sleep_for(chrono::milliseconds(1000 / 60));
	}

	return module::type::menu;
}
