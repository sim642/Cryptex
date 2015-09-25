#ifndef RS485_DONGLE_H
#define RS485_DONGLE_H

#include "device_manager.hpp"
#include <boost/asio.hpp>
#include "serial_device.hpp"

class rs485_dongle : public device_manager
{
	public:
		rs485_dongle(boost::asio::io_service &new_io, const std::string &dev);
		virtual ~rs485_dongle();

		void send(const int &id, const std::string &cmd);
		void send(const int &id, const std::string &cmd, const int &val);

		device_controller::recv_t send_recv(const int &id, const std::string &cmd);
		device_controller::recv_t send_recv(const int &id, const std::string &cmd, const int &val);

	private:
		virtual device_controller* request(const int &id);
		static device_controller::recv_t parse_recv(const std::string &line);

		boost::asio::io_service &io;
		boost::asio::serial_port port;
		serial_stream stream;
};

#endif // RS485_DONGLE_H
