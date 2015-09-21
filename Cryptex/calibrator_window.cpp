#include "calibrator_window.hpp"
#include <opencv2/opencv.hpp>

using namespace std;

calibrator_window::calibrator_window(cv::VideoCapture &new_capture, const std::string &new_env) : capture(new_capture), env(new_env)
{

}

calibrator_window::~calibrator_window()
{

}

void calibrator_window::calibrate(const std::string &color)
{
	string env_color = env + "/" + color;
	string window = "calibrate " + env_color;
	string filename = "./calibs/" + env_color + ".yml";

	cv::namedWindow(window);

	blob_finder blobber(filename);

	string channels = "HSV";
	blob_finder::bounds_t limits(179, 255, 255);
	for (size_t i = 0; i < channels.size(); i++)
	{
		cv::createTrackbar(string(1, channels[i]) + "min", window, &blobber.lower[i], limits[i]);
		cv::createTrackbar(string(1, channels[i]) + "max", window, &blobber.upper[i], limits[i]);
	}

	while (1)
	{
		cv::Mat frame;
		capture >> frame;

		cv::Mat mask;
		blobber.threshold(frame, mask);

		cv::Mat masked;
		frame.copyTo(masked, mask);

		cv::imshow(window, masked);

		char key = cv::waitKey(1000 / 60);
		switch (key)
		{
			case 27: // ESC
			case 'q':
				goto calibrate_quit;
		}
	}

calibrate_quit:

	cv::destroyWindow(window); // BUG: doesn't die immediately

	blobber.save_color(filename);
}
