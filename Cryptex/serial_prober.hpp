#ifndef SERIAL_PROBER_H
#define SERIAL_PROBER_H

#include <boost/asio.hpp>
#include "serial_controller.hpp"
#include <array>

class serial_prober
{
	public:
		serial_prober(boost::asio::io_service &new_io);
		virtual ~serial_prober();

		void add_device(const std::string &dev);

		serial_controller* operator[] (const int &id);
	private:
		boost::asio::io_service &io;

		std::array<serial_controller*, 256> controllers;
};

#endif // SERIAL_PROBER_H
