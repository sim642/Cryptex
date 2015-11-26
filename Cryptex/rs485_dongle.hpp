#ifndef RS485_DONGLE_H
#define RS485_DONGLE_H

#include "device_manager.hpp"
#include <boost/asio.hpp>
#include "serial_device.hpp"
#include <string>
#include <thread>
#include <mutex>
#include <atomic>

class rs485_dongle : public device_manager
{
	public:
		rs485_dongle(boost::asio::io_service &new_io, const std::string &dev);
		virtual ~rs485_dongle();

		void send(const int &id, const std::string &cmd);
		void send(const int &id, const std::string &cmd, const int &val);

		std::string read_line(const int &id);

	private:
		void receiver();
		virtual device_controller* request(const int &id);
		std::string read_line();
		static device_controller::recv_t parse_recv(const std::string &line);

		boost::asio::io_service &io;
		boost::asio::serial_port port;
		serial_stream stream;
		std::mutex stream_mut;

		std::thread thr;
		std::atomic_bool thr_running;
		std::vector<std::string> lines;
		std::mutex lines_mut;
};

#endif // RS485_DONGLE_H
