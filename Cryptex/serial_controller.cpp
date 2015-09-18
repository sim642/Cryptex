#include "serial_controller.hpp"

using namespace std;

serial_controller::serial_controller(boost::asio::serial_port &port) : serial_stream(port)
{
	port.set_option(boost::asio::serial_port_base::baud_rate(115200));
}

serial_controller::~serial_controller()
{

}

void serial_controller::send(const std::string &cmd)
{
	*this << cmd << endl;
}

void serial_controller::send(const std::string &cmd, const int &val)
{
	*this << cmd << val << endl;
}

serial_controller::recv_t serial_controller::send_recv(const std::string &cmd)
{
	send(cmd);

	std::string line;
	*this >> line;
	return parse_recv(line);
}

serial_controller::recv_t serial_controller::send_recv(const std::string &cmd, const int &val)
{
	send(cmd, val);

	std::string line;
	*this >> line;
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
