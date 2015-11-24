#include "device_controller.hpp"
#include <string>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/utility/in_place_factory.hpp>

using namespace std;


device_controller::device_controller()
{

}

device_controller::~device_controller()
{

}

device_controller::recv_t device_controller::recv(const std::string &reply)
{
	return parse_recv(read_line());
}

device_controller::recv_t device_controller::send_recv(const std::string &cmd, const std::string &reply)
{
	send(cmd);
	return recv(reply);
}

device_controller::recv_t device_controller::send_recv(const std::string &cmd, const std::string &reply, const int &val)
{
	send(cmd, val);
	return recv(reply);
}

device_controller::recv_t device_controller::parse_recv(const std::string &line)
{
	recv_t recv;
	recv = boost::in_place();

	if (!line.empty() && line.front() == '<' && line.back() == '>')
	{
		string inner(line.begin() + 1, line.end() - 1);
		vector<string> parts;
		boost::algorithm::split(parts, inner, boost::algorithm::is_any_of(":"));
		recv->first = parts[0];
		recv->second = parts[1];

		return recv;
	}

	return boost::none;
}
