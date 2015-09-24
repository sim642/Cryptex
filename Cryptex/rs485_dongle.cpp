#include "rs485_dongle.hpp"
#include "rs485_controller.hpp"
#include <string>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

using namespace std;

rs485_dongle::rs485_dongle(boost::asio::io_service &new_io, const std::string &dev) : io(new_io), port(io, dev), stream(port)
{
	port.set_option(boost::asio::serial_port_base::baud_rate(19200));

	for (auto &ptr : controllers)
		ptr = nullptr;
}

rs485_dongle::~rs485_dongle()
{
	for (auto &ptr : controllers)
		if (ptr)
			delete ptr;
}

rs485_controller* rs485_dongle::operator[] (const int &id)
{
	if (!controllers[id])
		controllers[id] = new rs485_controller(this, id);

	return controllers[id];
}

void rs485_dongle::send(const int &id, const std::string &cmd)
{
	stream << id << ":" << cmd << endl;
}

void rs485_dongle::send(const int &id, const std::string &cmd, const int &val)
{
	stream << id << ":" << cmd << val << endl;
}

rs485_dongle::recv_t rs485_dongle::send_recv(const int &id, const std::string &cmd)
{
	send(id, cmd);

	std::string line;
	stream >> line;
	return parse_recv(line);
}

rs485_dongle::recv_t rs485_dongle::send_recv(const int &id, const std::string &cmd, const int &val)
{
	send(id, cmd, val);

	std::string line;
	stream >> line;
	return parse_recv(line);
}

rs485_dongle::recv_t rs485_dongle::parse_recv(const std::string &line)
{
	recv_t recv;
	if (line.front() == '<' && line.back() == '>')
	{
		string inner(line.begin() + 1, line.end() - 1);
		vector<string> parts;
		boost::algorithm::split(parts, inner, boost::algorithm::is_any_of(":"));
		// TODO: check id
		recv.first = parts[1];
		recv.second = parts[2];
	}
	return recv;
}