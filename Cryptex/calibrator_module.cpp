#include "calibrator_module.hpp"

#include <iostream>
#include <stdexcept>
#include <opencv2/video.hpp>
#include "calibrator_window.hpp"
#include "global.hpp"
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <limits>

using namespace std;

calibrator_module::calibrator_module()
{

}

calibrator_module::~calibrator_module()
{

}

module::type calibrator_module::run(const module::type &prev_module)
{
	/*cv::VideoCapture capture(global::video_id);
	if (!capture.isOpened())
		throw runtime_error("capture could not be opened");*/

	multi_camera cams;
	cams.push_back(camera(2));
	cams.push_back(camera(1));

	calibrator_window calibrator(cams);

	string line;
	cin.ignore(numeric_limits<streamsize>::max(), '\n');
	while (cout << "color [params]: ", getline(cin, line))
	{
		if (line.empty())
			break;

		vector<string> parts;
		boost::algorithm::split(parts, line, boost::algorithm::is_any_of(" "));

		string color = parts[0], params = "";
		if (parts.size() >= 2)
			params = parts[1];

		calibrator.calibrate(color, params);
	}

	return module::type::menu;
}
