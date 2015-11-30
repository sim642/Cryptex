#include "serial_device.hpp"
#include <boost/asio.hpp>

serial_device::serial_device(boost::asio::serial_port &port) : serial_port(port)
{

}

serial_device::~serial_device()
{

}

std::streamsize serial_device::read(char *buf, std::streamsize n)
{
	auto &io = serial_port.get_io_service();
	io.reset();

	boost::asio::deadline_timer timeout(io);
	timeout.expires_from_now(boost::posix_time::millisec(1));
	timeout.async_wait([&](const boost::system::error_code &error)
	{
		if (error)
			return;

		serial_port.cancel();
	});

	size_t read = 0;
	boost::asio::async_read(serial_port, boost::asio::buffer(buf, n), [&](const boost::system::error_code &error, size_t bytes_transferred)
	{
		if (error || !bytes_transferred)
			return;

		read = bytes_transferred;
		timeout.cancel();
	});

	io.run();

	return read;
}

std::streamsize serial_device::write(const char *buf, std::streamsize n)
{
	return serial_port.write_some(boost::asio::buffer(buf, n));
}
