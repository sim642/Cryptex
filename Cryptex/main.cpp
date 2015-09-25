#include <iostream>
#include <boost/asio.hpp>
#include "serial_scanner.hpp"
#include "driver.hpp"
#include <chrono>
#include <thread>

#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include "blob_finder.hpp"
#include "rs485_dongle.hpp"

using namespace std;

int main()
{
	boost::asio::io_service io;

	rs485_dongle dongle(io, "/dev/ttyUSB0");
	device_controller *control = dongle[1];
	/*serial_scanner dongle(io, "ttyACM");
	device_controller *control = dongle[1];*/
	motor mot(control);
	while (1)
	{
		cout << mot.encoder() << endl;
		this_thread::sleep_for(chrono::milliseconds(2));
	}

	return 0;


	serial_scanner scanner(io, "ttyACM");

	driver d(scanner);

	cv::VideoCapture capture(1);
	if (!capture.isOpened())
	{
		cerr << "Failed to open capture" << endl;
		return EXIT_FAILURE;
	}

	blob_finder blobber("oranz.yml");
	cv::SimpleBlobDetector::Params params;
	params.filterByColor = true;
	params.blobColor = 255;
	params.filterByArea = true;
	params.minArea = 5.f;
	params.maxArea = numeric_limits<float>::max();
	params.filterByCircularity = false;
	params.filterByConvexity = false;
	params.filterByInertia = true;
	blobber.init_detector(params);

	bool find = false;

	cv::namedWindow("Remote");
	while (1)
	{
		cv::Mat frame;
		capture >> frame;

		cv::Mat keyframe;
		frame.copyTo(keyframe);

		if (find)
		{
			auto largest = blobber.largest(frame);
			if (largest.size >= 0.f) // if blob found
			{
				cv::circle(keyframe, largest.pt, largest.size / 2, cv::Scalar(255, 0, 255), 5);

				int diff = frame.cols / 2 - largest.pt.x;
				float factor = float(diff) / (frame.cols / 2);


				if (abs(factor) < 0.25)
				{
					float dist = (frame.rows - largest.pt.y) / float(frame.rows);
					d.straight(dist * 30);
				}
				else
				{
					d.rotate(factor * 50);
				}
			}
		}

		imshow("Remote", keyframe);

		char key = cv::waitKey(1000 / 60);
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

			case 'e':
				find ^= 1;
				break;
		}
	}

quit:

	return EXIT_SUCCESS;
}
