#include "test_module.hpp"

#include <iostream>
#include <boost/asio.hpp>
#include "serial_scanner.hpp"
#include "rs485_dongle.hpp"
#include "motor.hpp"
#include <chrono>
#include <thread>

#include "driver.hpp"

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

	for (int i = 0; i < 4; i++)
	{
		d.omni(50, i * 90);
		this_thread::sleep_for(chrono::seconds(1));
	}

	//device_controller *control = dongle[1];
	/*serial_scanner dongle(io, "ttyACM");
	device_controller *control = dongle[1];*/
	/*motor mot(control);
	while (1)
	{
		cout << mot.encoder() << endl;
		this_thread::sleep_for(chrono::milliseconds(2));
	}*/

	return module::type::menu;
}