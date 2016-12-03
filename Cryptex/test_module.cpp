#include "test_module.hpp"

#include <iostream>
#include <boost/asio.hpp>
#include <chrono>
#include <thread>
#include <vector>
#include <cctype>

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
#include "border_detector.hpp"

#include "mbed_main_controller.hpp"
#include "mbed_driver.hpp"

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
	mbed_main_controller m(io, "/dev/ttyACM1");
	mbed_driver d(m);

	m.failsafe(false);

	while (1)
	{
        int speed, angle, rotate;
        cin >> speed >> angle >> rotate;


		m.dribbler(1);
		this_thread::sleep_for(chrono::milliseconds(500));

		d.omni(speed, angle, rotate);
		this_thread::sleep_for(chrono::milliseconds(5000));

		m.dribbler(0);
		d.stop();
	}

	return module::type::menu;
}
