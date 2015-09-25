#ifndef SERIAL_SCANNER_H
#define SERIAL_SCANNER_H

#include "device_manager.hpp"
#include <boost/asio.hpp>
#include "serial_controller.hpp"
#include <array>

class serial_scanner : public device_manager
{
	public:
		serial_scanner(boost::asio::io_service &new_io);
		virtual ~serial_scanner();

		void add_device(const std::string &dev);
		void scan_devices();

		virtual serial_controller* operator[] (const int &id);

	private:
		boost::asio::io_service &io;

		std::array<serial_controller*, 256> controllers;
};

#endif // SERIAL_SCANNER_H
