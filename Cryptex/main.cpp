#include <iostream>
#include <boost/asio.hpp>
#include "motor.hpp"
#include <vector>
#include <boost/ptr_container/ptr_vector.hpp>

using namespace std;

int main()
{
	boost::asio::io_service io;

	vector<boost::asio::serial_port> ports;
	ports.emplace_back(io, "/dev/ttyACM0");

	boost::ptr_vector<serial_controller> controllers;
	for (auto &port : ports)
		controllers.push_back(new serial_controller(port));

	vector<motor> motors;
	for (auto &controller : controllers)
		motors.emplace_back(controller);

	for (auto &motor : motors)
		motor.drive(20);
	return 0;
}
