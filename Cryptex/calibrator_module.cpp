#include "calibrator_module.hpp"

#include <iostream>
#include <opencv2/video.hpp>
#include "calibrator_window.hpp"

using namespace std;

calibrator_module::calibrator_module()
{

}

calibrator_module::~calibrator_module()
{

}

module::type calibrator_module::run(const module::type &prev_module)
{
	cv::VideoCapture capture(0);
	if (!capture.isOpened())
	{
		cerr << "Failed to open capture" << endl;
		return module::type::exit;
	}

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

	return module::type::exit;
}
