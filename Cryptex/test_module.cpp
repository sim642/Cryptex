#include "test_module.hpp"

#include <iostream>
#include <boost/asio.hpp>
#include <chrono>
#include <thread>

#include "global.hpp"
#include "serial_scanner.hpp"
#include "rs485_dongle.hpp"
#include "merge_manager.hpp"
#include "driver.hpp"

#include "serial_device.hpp"
#include "srf_dongle.hpp"

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

	merge_manager manager;
	manager.add_manager(new serial_scanner(io, "ttyACM"));
	manager.add_manager(new rs485_dongle(io, "/dev/ttyUSB0"));

	driver d(manager);

	srf_dongle srf(io, "/dev/ttyACM0");

	while (1)
	{
		/*auto data = srf.recv_raw();
		if (!data.empty())
			cout << ">" << data << "<" << endl;*/
		auto data = srf.recv_parsed();
		if (get<0>(data))
		{
			char field, target;
			string cmd;
			tie(field, target, cmd) = data;

			//cout << get<0>(data) << " " << get<1>(data) << " " << get<2>(data) << endl;]

			if ((field == 'A' || field == 'B') && ((target >= 'A' && target <= 'D') || target == 'X') && (cmd == "START" || cmd == "STOP"))
				cout << field << " " << target << "  " << cmd << endl;
		}

		this_thread::sleep_for(chrono::milliseconds(100));
	}

	/*boost::asio::serial_port port(io, "/dev/ttyACM0");
	port.set_option(boost::asio::serial_port_base::baud_rate(9600));

	serial_stream stream(port);

	while (1)
	{
		cout << stream.peek() << endl;

		this_thread::sleep_for(chrono::milliseconds(100));
	}*/



	return module::type::menu;
}
