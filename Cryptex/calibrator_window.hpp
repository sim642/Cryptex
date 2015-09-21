#ifndef CALIBRATOR_WINDOW_H
#define CALIBRATOR_WINDOW_H

#include "blob_finder.hpp"
#include <opencv2/video.hpp>
#include <string>

class calibrator_window
{
	public:
		calibrator_window(cv::VideoCapture &new_capture, const std::string &new_env);
		virtual ~calibrator_window();

		void calibrate(const std::string &color, const std::string &params);
	private:
		cv::VideoCapture capture;
		std::string env;
};

#endif // CALIBRATOR_WINDOW_H
