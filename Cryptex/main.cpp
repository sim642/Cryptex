#include <iostream>
#include <boost/asio.hpp>
#include "serial_scanner.hpp"
#include "driver.hpp"
#include <chrono>
#include <thread>

#include <opencv2/highgui.hpp>

using namespace std;

int main()
{
	boost::asio::io_service io;

	serial_scanner scanner(io);
	scanner.scan_devices();

	driver d(scanner);


	cv::namedWindow("Remote");
	while (1)
	{
		char key = cv::waitKey(1000 / 30);
		switch (key)
		{
			case 27:
			case 'q':
				goto quit;

			case 'w':
				d.straight(75);
				break;

			case 's':
				d.straight(-50);
				break;

			case 'a':
				d.rotate(30);
				break;

			case 'd':
				d.rotate(-30);
				break;

			case ' ':
				d.stop();
				break;
		}
	}

quit:

	return 0;
}
