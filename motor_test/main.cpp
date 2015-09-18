#include <iostream>
#include <boost/asio.hpp>
#include <boost/iostreams/stream.hpp>
#include "serial_device.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace std;

int main()
{
    boost::asio::io_service io;
    boost::asio::serial_port port(io, "/dev/ttyACM1");
    port.set_option(boost::asio::serial_port_base::baud_rate(115200));

    serial_device dev(port);
    boost::iostreams::stream<serial_device> serial(dev);

    serial << "?" << endl;
    string text;
    serial >> text;
    cout << text << endl;

    while (true)
    {
        serial << "sd10" << endl;

        boost::asio::deadline_timer timer(io, boost::posix_time::seconds(1));
        timer.wait();
    }
    return 0;
}
