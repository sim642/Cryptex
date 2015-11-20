#include "device_controller.hpp"
#include <string>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <chrono>
#include <thread>

using namespace std;


device_controller::device_controller()
{

}

device_controller::~device_controller()
{

}

device_controller::recv_t device_controller::recv(const std::string &reply)
{
	cout << "rec1" << endl;
	chrono::high_resolution_clock::time_point start = chrono::high_resolution_clock::now();
	recv_t recv;
	do
	{
		recv = parse_recv(read_line());
		cout << "rec_: " << recv.first << ", " << recv.second << endl;
	}
	while (recv.first != reply && chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - start).count() < 100);
	cout << "rec2: " << recv.first << ", " << recv.second << endl;

	return recv;
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
	if (line.front() == '<' && line.back() == '>')
	{
		string inner(line.begin() + 1, line.end() - 1);
		vector<string> parts;
		boost::algorithm::split(parts, inner, boost::algorithm::is_any_of(":"));
		recv.first = parts[0];
		recv.second = parts[1];
	}
	return recv;
}
