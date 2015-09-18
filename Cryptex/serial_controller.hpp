#ifndef SERIAL_CONTROLLER_H
#define SERIAL_CONTROLLER_H

#include "serial_device.hpp"
#include <string>
#include <utility>

class serial_controller
{
	public:
		typedef std::pair<std::string, std::string> recv_t;

		serial_controller(boost::asio::io_service &io, const std::string &dev);
		virtual ~serial_controller();

		void send(const std::string &cmd);
		void send(const std::string &cmd, const int &val);

		recv_t send_recv(const std::string &cmd);
		recv_t send_recv(const std::string &cmd, const int &val);

	private:
		static recv_t parse_recv(const std::string &line);

		boost::asio::serial_port port;
		serial_stream stream;
};

#endif // SERIAL_CONTROLLER_H
