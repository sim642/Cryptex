#ifndef SERIAL_DEVICE_H
#define SERIAL_DEVICE_H

#include <boost/iostreams/stream.hpp>
#include <boost/asio/serial_port.hpp>

/*
Shamelessly stolen from http://lists.boost.org/boost-users/2009/03/45664.php
*/

class serial_device
{
	public:
		typedef char char_type;
		typedef boost::iostreams::bidirectional_device_tag category;

		serial_device(boost::asio::serial_port &port);
		virtual ~serial_device();

		std::streamsize read(char *buf, std::streamsize n);
		std::streamsize write(const char *buf, std::streamsize n);

	private:
		boost::asio::serial_port &serial_port;
};

typedef boost::iostreams::stream<serial_device> serial_stream;

#endif // SERIAL_DEVICE_H
