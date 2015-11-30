#include "serial_device.hpp"

serial_device::serial_device(boost::asio::serial_port &port) : serial_port(port)
{

}

serial_device::~serial_device()
{

}

std::streamsize serial_device::read(char *buf, std::streamsize n)
{
	return serial_port.read_some(boost::asio::buffer(buf, n));
}

std::streamsize serial_device::write(const char *buf, std::streamsize n)
{
	return serial_port.write_some(boost::asio::buffer(buf, n));
}
