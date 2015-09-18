#include "serial_prober.hpp"
#include <string>
#include <iostream>

using namespace std;

serial_prober::serial_prober(boost::asio::io_service &new_io) : io(new_io)
{
	for (auto &ptr : controllers)
		ptr = nullptr;
}

serial_prober::~serial_prober()
{
	for (auto &ptr : controllers)
		if (ptr)
			delete ptr;
}


void serial_prober::add_device(const std::string &dev)
{
	serial_controller *controller = new serial_controller(io, dev);

	int id = stoi(controller->send_recv("?").second);
	cout << dev << " : " << id << endl;
	controllers[id] = controller;
}

serial_controller* serial_prober::operator[] (const int &id)
{
	return controllers[id];
}
