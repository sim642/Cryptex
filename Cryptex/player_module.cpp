#include "player_module.hpp"

#include <iostream>

#include <boost/asio.hpp>
#include "serial_scanner.hpp"
#include "driver.hpp"

#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include "blob_finder.hpp"

#include "global.hpp"

using namespace std;

player_module::player_module()
{

}

player_module::~player_module()
{

}

module::type player_module::run(const module::type &prev_module)
{
	boost::asio::io_service io;
	serial_scanner scanner(io, "ttyACM");

	driver d(scanner);

	cv::VideoCapture capture(global::video_id);
	if (!capture.isOpened())
		throw runtime_error("capture could not be opened");

	blob_finder blobber("oranz", "ball");

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
				return module::type::menu;

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
}
