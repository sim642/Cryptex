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
	multi_camera cams = load_multi_camera();

	cv::namedWindow("lines");
	cv::namedWindow("canny");

	blob_finder blobber("must");
	blob_finder blobber2("valge");
	//blobber.opening = true;
	border_detector borders(blobber, blobber2, "border");

	cv::createTrackbar("linethres", "lines", &borders.linethres, 500);
	cv::createTrackbar("linelength", "lines", &borders.linelength, 500);
	cv::createTrackbar("linegap", "lines", &borders.linegap, 500);

	cams[0].update();

	while (1)
	{
		cv::Mat multi_display(cams[0].frame.size().height, cams[0].frame.size().width * cams.size(), CV_8UC3, cv::Scalar(0, 0, 0));
		cv::Mat multi_canny(cams[0].frame.size().height, cams[0].frame.size().width * cams.size(), CV_8UC1, cv::Scalar(0));

		for (int i = 0; i < cams.size(); i++)
		{
			auto &cam = cams[i];

			auto display = multi_display(cv::Rect(i * cam.frame.size().width, 0, cam.frame.size().width, cam.frame.size().height));
			auto canny = multi_canny(cv::Rect(i * cam.frame.size().width, 0, cam.frame.size().width, cam.frame.size().height));

			cam.update();
			cam.frame.copyTo(display);

			lines_t lines;
			borders.detect(cam, lines);

			borders.draw(display);
			borders.canny.copyTo(canny);
		}

		cv::imshow("lines", multi_display);
		cv::imshow("canny", multi_canny);

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
				cv::imwrite("pics/ss.jpg", multi_display);
				break;

			case 'd':
				borders.default_lines();
				break;
		}
	}

	return module::type::menu;
}
