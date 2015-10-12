#include "srf_dongle.hpp"
#include <iomanip>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <functional>

using namespace std;

srf_dongle::srf_dongle(boost::asio::io_service &io, const std::string &dev) : port(io, dev), stream(port), thr(bind(&srf_dongle::receiver, this))
{
	port.set_option(boost::asio::serial_port_base::baud_rate(9600));
}

srf_dongle::~srf_dongle()
{

}

void srf_dongle::send(std::string raw)
{
	lock_guard<mutex> lock(stream_mut);
	stream << raw << flush;
}

void srf_dongle::send(char start, std::string id, std::string cmd)
{
	lock_guard<mutex> lock(stream_mut);
	stream << setw(1) << start << setw(2) << id << setw(9) << setfill('-') << left << cmd << flush;
}

void srf_dongle::send(char field, char target, std::string cmd)
{
	send('a', string(1, field) + string(1, target), cmd);
}

std::string srf_dongle::recv_raw()
{
	lock_guard<mutex> lock(recvd_mut);
	if (recvd.empty())
		return "";
	else if (recvd.front().size() >= 12)
	{
		string ret = recvd.front().substr(0, 12);
		recvd.erase(recvd.begin()); // pop
		return ret;
	}

	return "";
}

std::tuple<char, std::string, std::string> srf_dongle::recv()
{
	string buf = recv_raw();
	if (buf.empty())
		return make_tuple('\0', "", "");
	else
		return make_tuple(buf[0], buf.substr(1, 2), boost::algorithm::trim_right_copy_if(buf.substr(3), boost::algorithm::is_any_of("-")));
}

std::tuple<char, char, std::string> srf_dongle::recv_parsed()
{
	auto r = recv();
	if (get<0>(r) == '\0')
		return make_tuple('\0', '\0', "");
	else
		return make_tuple(get<1>(r)[0], get<1>(r)[1], get<2>(r));
}

void srf_dongle::receiver()
{
	while (1)
	{
		char ch;

		{
			lock_guard<mutex> lock(stream_mut);
			ch = stream.get();
		}

		{
			lock_guard<mutex> lock(recvd_mut);

			if (ch == 'a') // packet start
			{
				if (!recvd.empty() && recvd.back().size() < 12)
					recvd.back().append(12 - recvd.back().size(), '-');

				recvd.push_back("a");
			}
			else if (!recvd.empty())
			{
				recvd.back().append(1, ch);
			}
		}
	}
}
