#include "controller.hpp"
#include <string>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

using namespace std;


controller::controller()
{

}

controller::~controller()
{

}

controller::recv_t controller::parse_recv(const std::string &line)
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
