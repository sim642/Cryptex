#include <iostream>
#include <boost/asio.hpp>
#include "motor.hpp"
#include <vector>
#include <boost/ptr_container/ptr_vector.hpp>
#include "serial_prober.hpp"
#include "driver.hpp"

using namespace std;

int main()
{
	boost::asio::io_service io;

	serial_prober prober(io);
	prober.add_device("/dev/ttyACM3");
	prober.add_device("/dev/ttyACM4");
	prober.add_device("/dev/ttyACM5");

	/*vector<boost::asio::serial_port> ports;
	ports.emplace_back(io, "/dev/ttyACM4");
	ports.emplace_back(io, "/dev/ttyACM5");
	ports.emplace_back(io, "/dev/ttyACM6");

	boost::ptr_vector<serial_controller> controllers;
	for (auto &port : ports)
		controllers.push_back(new serial_controller(port));

	vector<motor> motors;
	for (auto &controller : controllers)
		motors.emplace_back(controller);*/

	for (int i = 1; i <= 3; i++)
	{
		cout << "id " << prober[i]->send_recv("?").second << endl;
	}

	/*vector<motor> motors;
	motors.emplace_back(*prober[1]);
	motors.emplace_back(*prober[2]);
	motors.emplace_back(*prober[3]);*/

	/*for (auto &motor : motors)
	{
		//cout << "id2 " << motor.controller.send_recv("?").second << endl;
		motor.drive(-100);
	}*/

	/*motors[0].drive(-30);
	motors[1].drive(0);
	motors[2].drive(30);*/

	driver d(prober);
	d.straight(30);
	return 0;
}
