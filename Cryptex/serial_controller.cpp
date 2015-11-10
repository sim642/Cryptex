#include "serial_controller.hpp"
#include <string>

using namespace std;

serial_controller::serial_controller(boost::asio::io_service &io, const std::string &dev) : port(io, dev), stream(port)
{
	port.set_option(boost::asio::serial_port_base::baud_rate(115200));
}

serial_controller::~serial_controller()
{

}

void serial_controller::send(const std::string &cmd)
{
	stream << cmd << endl;
}

void serial_controller::send(const std::string &cmd, const std::string &val)
{
	stream << cmd << val << endl;
}

int serial_controller::id()
{
	return stoi(send_recv("?", "id").second);
}

int serial_controller::id(const int &new_id)
{
	send("id", new_id);
	return id();
}

std::string serial_controller::read_line()
{
	std::string line;
	stream >> line;
	return line;
}
