#include "calibrator_window.hpp"
#include <opencv2/opencv.hpp>
#include <utility>
#include "global.hpp"

using namespace std;

calibrator_window::calibrator_window(cv::VideoCapture &new_capture) : capture(new_capture)
{

}

calibrator_window::~calibrator_window()
{

}

void calibrator_window::calibrate(const std::string &color, const std::string &params)
{
	bool param = !params.empty();

	string env_color = global::env + "/" + color;
	string win_color = "calibrate " + env_color;

	string win_params = "calibrate " + params;

	cv::namedWindow(win_color);
	if (param)
		cv::namedWindow(win_params);

	blob_finder blobber(color);
	if (param)
		blobber.detector.load_params(params);

	string channels = "HSV";
	blob_finder::bounds_t limits(179, 255, 255);
	for (size_t i = 0; i < channels.size(); i++)
	{
		cv::createTrackbar(string(1, channels[i]) + "min", win_color, &blobber.lower[i], limits[i]);
		cv::createTrackbar(string(1, channels[i]) + "max", win_color, &blobber.upper[i], limits[i]);
	}
	cv::createTrackbar("struct_size", win_color, &blobber.struct_size, 25);

	int minarea = blobber.detector.area.first;
	if (param)
	{
		cv::createTrackbar("minarea", win_params, &minarea, 10000);
	}

	while (1)
	{
		cv::Mat frame;
		capture >> frame;

		cv::Mat mask;
		blobber.threshold(frame, mask);

		cv::Mat masked;
		frame.copyTo(masked, mask);

		cv::imshow(win_color, masked);
		if (param)
		{
			blobber.detector.area.first = minarea;

			blobs_t blobs;
			blobber.detect(mask, blobs);

			cv::Mat key_img;
			cv::cvtColor(mask, key_img, CV_GRAY2BGR);
			for (auto &b : blobs)
			{
				cv::circle(key_img, b.center, b.radius, cv::Scalar(255, 0, 255));
				cv::rectangle(key_img, b.rect, cv::Scalar(255, 255, 0));
			}

			cv::imshow(win_params, key_img);
		}

		char key = cv::waitKey(1000 / 60);
		switch (key)
		{
			case 27: // ESC
			case 'q':
				goto calibrate_quit;
		}
	}

calibrate_quit:

	cv::destroyWindow(win_color); // BUG: doesn't die immediately
	if (param)
		cv::destroyWindow(win_params);

	blobber.save_color(color);
	if (param)
		blobber.detector.save_params(params);
}
