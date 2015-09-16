#include <iostream>
#include <boost/asio.hpp>

using namespace std;

int main()
{
    boost::asio::io_service io;
    boost::asio::serial_port serial(io, "/dev/ttyACM1");
    serial.set_option(boost::asio::serial_port_base::baud_rate(115200));

    string str = "sd-10\n";
	boost::asio::write(serial, boost::asio::buffer(str.c_str(), str.length()));
    return 0;
}
