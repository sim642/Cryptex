#ifndef RS485_DONGLE_H
#define RS485_DONGLE_H

#include "device_manager.hpp"
#include <boost/asio.hpp>
#include <array>
#include "serial_device.hpp"
#include "rs485_controller.hpp"

class rs485_dongle : public device_manager
{
	public:
		typedef std::pair<std::string, std::string> recv_t;

		rs485_dongle(boost::asio::io_service &new_io, const std::string &dev);
		virtual ~rs485_dongle();

		virtual rs485_controller* operator[] (const int &id);

		void send(const int &id, const std::string &cmd);
		void send(const int &id, const std::string &cmd, const int &val);

		recv_t send_recv(const int &id, const std::string &cmd);
		recv_t send_recv(const int &id, const std::string &cmd, const int &val);

	private:
		static recv_t parse_recv(const std::string &line);

		boost::asio::io_service &io;
		boost::asio::serial_port port;
		serial_stream stream;

		std::array<rs485_controller*, 256> controllers;
};

#endif // RS485_DONGLE_H
