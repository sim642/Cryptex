#include "test_module.hpp"

#include <iostream>
#include <boost/asio.hpp>
#include <chrono>
#include <thread>

#include "util.hpp"

#include "global.hpp"
#include "device_id.hpp"

#include "rs485_dongle.hpp"
#include "driver.hpp"
#include "main_controller.hpp"
#include "serial_controller.hpp"

#include "calibrator_window.hpp"
#include "blob_finder.hpp"
#include "blob_tracker.hpp"

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
	main_controller m(dongle[device_id::main]);
	/*serial_controller s(io, "/dev/ttyACM1");
	main_controller m(&s);*/

	for (int i = 0;; i++)
	{
		m.ping();
		//this_thread::sleep_for(chrono::milliseconds(50));

		//cout << i << endl;
		cout << i << ": " << flush;
		cout << m.button(1) << " " << flush;
		cout << m.ball() << " " << endl;

		//this_thread::sleep_for(chrono::milliseconds(1000 / 30));
	}

	return module::type::menu;
}
