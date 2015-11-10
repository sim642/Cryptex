#ifndef SERIAL_CONTROLLER_H
#define SERIAL_CONTROLLER_H

#include "device_controller.hpp"
#include "serial_device.hpp"

class serial_controller : public device_controller
{
	public:
		serial_controller(boost::asio::io_service &io, const std::string &dev);
		virtual ~serial_controller();

		virtual void send(const std::string &cmd);
		virtual void send(const std::string &cmd, const std::string &val);
		using device_controller::send;

		int id();
		int id(const int &new_id);

	private:
		virtual std::string read_line();

		boost::asio::serial_port port;
		serial_stream stream;
};

#endif // SERIAL_CONTROLLER_H
