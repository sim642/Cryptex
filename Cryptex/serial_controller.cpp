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

void serial_controller::send(const std::string &cmd, const int &val)
{
	stream << cmd << val << endl;
}

serial_controller::recv_t serial_controller::send_recv(const std::string &cmd)
{
	send(cmd);

	std::string line;
	stream >> line;
	return parse_recv(line);
}

serial_controller::recv_t serial_controller::send_recv(const std::string &cmd, const int &val)
{
	send(cmd, val);

	std::string line;
	stream >> line;
	return parse_recv(line);
}

int serial_controller::id()
{
	return stoi(send_recv("?").second);
}

int serial_controller::id(const int &new_id)
{
	send("id", new_id);
	return id();
}
