#ifndef SRF_DONGLE_H
#define SRF_DONGLE_H

#include "serial_device.hpp"
#include <string>
#include <queue>
#include <tuple>

class srf_dongle
{
	public:
		srf_dongle(boost::asio::io_service &io, const std::string &dev);
		virtual ~srf_dongle();

		void send(char start, std::string id, std::string cmd);
		void send(char field, char target, std::string cmd);

		std::string recv_raw();
		std::tuple<char, std::string, std::string> recv();
		std::tuple<char, char, std::string> recv_parsed();

	private:
		boost::asio::serial_port port;
		serial_stream stream;

		std::string buffer;
};

#endif // SRF_DONGLE_H
