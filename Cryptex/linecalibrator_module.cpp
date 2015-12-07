#include "linecalibrator_module.hpp"

#include <string>
#include <vector>
#include <opencv2/opencv.hpp>

#include "global.hpp"
#include "blob_finder.hpp"
#include "border_detector.hpp"

using namespace std;

linecalibrator_module::linecalibrator_module()
{

}

linecalibrator_module::~linecalibrator_module()
{

}

module::type linecalibrator_module::run(const module::type &prev_module)
{
	cv::VideoCapture capture(global::video_id);

	cv::namedWindow("lines");
	cv::namedWindow("canny");

	blob_finder blobber("valge");
	//blobber.opening = true;
	border_detector borders(blobber, "border");

	cv::createTrackbar("linethres", "lines", &borders.linethres, 500);
	cv::createTrackbar("linelength", "lines", &borders.linelength, 500);
	cv::createTrackbar("linegap", "lines", &borders.linegap, 500);

	int i = 0;
	while (1)
	{
		cv::Mat frame;
		capture >> frame;

		cv::Mat display;
		frame.copyTo(display);

		lines_t lines;
		borders.detect(frame, lines);

		borders.draw(display);

		cv::imshow("lines", display);
		cv::imshow("canny", borders.canny);

		char key = cv::waitKey(1000 / 30);
		switch (key)
		{
			case 'q':
			{
				cv::destroyWindow("lines");
				cv::destroyWindow("canny");

				borders.save_lines("border");
				return module::type::menu;
			}

			case 's':
				cv::imwrite("pics/ss.jpg", display);
				break;

			case 'd':
				borders.default_lines();
				break;

			default:
				if (isdigit(key))
				{
					i = key - '1';
				}
				break;
		}
	}

	return module::type::menu;
}
