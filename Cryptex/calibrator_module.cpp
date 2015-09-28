#include "calibrator_module.hpp"

#include <iostream>
#include <stdexcept>
#include <opencv2/video.hpp>
#include "calibrator_window.hpp"
#include "global.hpp"

using namespace std;

calibrator_module::calibrator_module()
{

}

calibrator_module::~calibrator_module()
{

}

module::type calibrator_module::run(const module::type &prev_module)
{
	cv::VideoCapture capture(global::video_id);
	if (!capture.isOpened())
		throw runtime_error("capture could not be opened");

	calibrator_window calibrator(capture);
	string color, params;
	while (cout << "calib: ", cin >> color >> params)
	{
		if (color == "_")
			break;

		if (params == "_")
			params = "";

		calibrator.calibrate(color, params);
	}

	return module::type::menu;
}
