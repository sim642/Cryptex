#include "calibrator_window.hpp"
#include <opencv2/opencv.hpp>
#include <utility>
#include "global.hpp"

using namespace std;

calibrator_window::calibrator_window(multi_camera &new_cams) : cams(new_cams)
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
	blob_finder::bounds_t limits(180, 256, 256);
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

	cams[0].update();

	while (1)
	{
		cv::Mat masked(cams[0].frame.size().height, cams[0].frame.size().width * cams.size(), CV_8UC3, cv::Scalar(0, 0, 0));

		for (int i = 0; i < cams.size(); i++)
		{
			auto &cam = cams[i];

			cam.update();

			cv::Mat mask;
			blobber.threshold(cam, mask);

			cam.frame.copyTo(masked(cv::Rect(i * cam.frame.size().width, 0, cam.frame.size().width, cam.frame.size().height)), mask);
		}

		cv::imshow(win_color, masked);
		if (param)
		{
			blobber.detector.area.first = minarea;

			cv::Mat key_img2(cams[0].frame.size().height, cams[0].frame.size().width * cams.size(), CV_8UC3, cv::Scalar(0, 0, 0));

			for (int i = 0; i < cams.size(); i++)
			{
				auto &cam = cams[i];

				auto key_img = key_img2(cv::Rect(i * cam.frame.size().width, 0, cam.frame.size().width, cam.frame.size().height));

				cv::Mat mask;
				blobber.threshold(cam, mask);

				blobs_t blobs;
				blobber.detect(cam, mask, blobs);


				cv::cvtColor(mask, key_img, CV_GRAY2BGR);
				for (auto &b : blobs)
				{
					cv::circle(key_img, b.center, b.radius, cv::Scalar(255, 0, 255));
					cv::rectangle(key_img, b.rect, cv::Scalar(255, 255, 0));
				}
			}

			cv::imshow(win_params, key_img2);
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
