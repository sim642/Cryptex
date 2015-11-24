#include "rs485_dongle.hpp"
#include "rs485_controller.hpp"
#include <string>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/utility/in_place_factory.hpp>

using namespace std;

rs485_dongle::rs485_dongle(boost::asio::io_service &new_io, const std::string &dev) : io(new_io), port(io, dev), stream(port)
{
	port.set_option(boost::asio::serial_port_base::baud_rate(19200));
}

rs485_dongle::~rs485_dongle()
{

}

void rs485_dongle::send(const int &id, const std::string &cmd)
{
	stream << id << ":" << cmd << endl;
}

void rs485_dongle::send(const int &id, const std::string &cmd, const int &val)
{
	stream << id << ":" << cmd << val << endl;
}

std::string rs485_dongle::read_line(const int &id)
{
	device_controller::recv_t recv = parse_recv(read_line());

	if (recv && recv->first == to_string(id))
		return "<" + recv->second + ">"; // imitate serial brackets
	else
		return "";
}

device_controller* rs485_dongle::request(const int &id)
{
	return new rs485_controller(this, id);
}

device_controller::recv_t rs485_dongle::parse_recv(const std::string &line)
{
	device_controller::recv_t recv;
	recv = boost::in_place();

	if (!line.empty() && line.front() == '<' && line.back() == '>')
	{
		string inner(line.begin() + 1, line.end() - 1);
		vector<string> parts;
		boost::algorithm::split(parts, inner, boost::algorithm::is_any_of(":"));
		recv->first = parts[0];
		recv->second = boost::algorithm::join(vector<string>(parts.begin() + 1, parts.end()), ":"); // TODO: only one split

		return recv;
	}

	return boost::none;
}

std::string rs485_dongle::read_line()
{
	std::string line;

	io.reset();

	boost::asio::deadline_timer timeout(io);
	timeout.expires_from_now(boost::posix_time::milliseconds(5));
	timeout.async_wait([&](const boost::system::error_code &error)
	{
		if (error)
			return;

		port.cancel();
	});

	boost::asio::async_read_until(port, buf, "\n", [&](const boost::system::error_code &error, size_t bytes_transferred)
	{
		if (error || !bytes_transferred)
			return;

		timeout.cancel();

		istream is(&buf);
		getline(is, line);
	});

	io.run_one();

	return line;
}
