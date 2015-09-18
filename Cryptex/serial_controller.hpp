#ifndef SERIAL_CONTROLLER_H
#define SERIAL_CONTROLLER_H

#include "serial_device.hpp"
#include <string>
#include <utility>

class serial_controller : public serial_stream
{
	public:
		typedef std::pair<std::string, std::string> recv_t;

		serial_controller(boost::asio::serial_port &port);
		virtual ~serial_controller();

		void send(const std::string &cmd);
		void send(const std::string &cmd, const int &val);

		recv_t send_recv(const std::string &cmd);
		recv_t send_recv(const std::string &cmd, const int &val);

	private:
		recv_t parse_recv(const std::string &line);
};

#endif // SERIAL_CONTROLLER_H
