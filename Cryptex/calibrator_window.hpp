#ifndef CALIBRATOR_WINDOW_H
#define CALIBRATOR_WINDOW_H

#include "blob_finder.hpp"
#include <opencv2/video.hpp>
#include <string>

class calibrator_window
{
	public:
		calibrator_window(multi_camera &new_cams);
		virtual ~calibrator_window();

		void calibrate(const std::string &color, const std::string &params);
	private:
		multi_camera &cams;
};

#endif // CALIBRATOR_WINDOW_H
