#include "serial_scanner.hpp"
#include "serial_controller.hpp"
#include <boost/filesystem.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <iostream>

using namespace std;

serial_scanner::serial_scanner(boost::asio::io_service &new_io) : io(new_io)
{

}

serial_scanner::serial_scanner(boost::asio::io_service &new_io, const std::string &tty) : serial_scanner(new_io)
{
	scan_devices(tty);
}

serial_scanner::~serial_scanner()
{

}


void serial_scanner::add_device(const std::string &dev)
{
	serial_controller *controller = new serial_controller(io, dev);

	int id = controller->id();
	cout << dev << " : " << id << endl;
	controllers[id] = controller;
}

void serial_scanner::scan_devices(const std::string &tty)
{
	boost::filesystem::path devs("/dev");
	for (boost::filesystem::directory_iterator it(devs); it != boost::filesystem::directory_iterator(); ++it)
	{
		if (boost::algorithm::starts_with(it->path().filename().string(), tty))
		{
			cout << it->path().string() << endl;
			add_device(it->path().string());
		}
	}
}

