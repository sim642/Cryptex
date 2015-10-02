#ifndef SERIAL_SCANNER_H
#define SERIAL_SCANNER_H

#include "device_manager.hpp"
#include <boost/asio.hpp>
#include <string>

class serial_scanner : public device_manager
{
	public:
		serial_scanner(boost::asio::io_service &new_io);
		serial_scanner(boost::asio::io_service &new_io, const std::string &tty);
		virtual ~serial_scanner();

		void add_device(const std::string &dev);
		void scan_devices(const std::string &tty);

	private:
		boost::asio::io_service &io;
};

#endif // SERIAL_SCANNER_H
