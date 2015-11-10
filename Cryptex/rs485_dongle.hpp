#ifndef RS485_DONGLE_H
#define RS485_DONGLE_H

#include "device_manager.hpp"
#include <boost/asio.hpp>
#include "serial_device.hpp"
#include <string>

class rs485_dongle : public device_manager
{
	public:
		rs485_dongle(boost::asio::io_service &new_io, const std::string &dev);
		virtual ~rs485_dongle();

		void send(const int &id, const std::string &cmd);
		void send(const int &id, const std::string &cmd, const std::string &val);

		std::string read_line(const int &id);

	private:
		virtual device_controller* request(const int &id);
		std::string read_line();
		static device_controller::recv_t parse_recv(const std::string &line);

		boost::asio::io_service &io;
		boost::asio::serial_port port;
		serial_stream stream;
};

#endif // RS485_DONGLE_H
