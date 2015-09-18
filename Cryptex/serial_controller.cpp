#include "serial_controller.hpp"

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

serial_controller::recv_t serial_controller::parse_recv(const std::string &line)
{
	recv_t recv;
	if (line.front() == '<' && line.back() == '>')
	{
		auto it = std::find(line.begin(), line.end(), ':');
		recv.first = std::string(line.begin() + 1, it);
		recv.second = std::string(it + 1, line.end() - 1);
	}
	return recv;
}
