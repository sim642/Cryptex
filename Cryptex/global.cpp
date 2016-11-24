#include "global.hpp"
#include <opencv2/opencv.hpp>
#include <cstdlib>

namespace global
{
	std::string env;
	int video_id;
	char field, id;
	bool dribbler, coilgun, referee;

	std::string camera;
	float hfov, vfov, h, alpha;

	std::string calib_filename(const std::string &name, const std::string &ext)
	{
		return "./calibs/" + name + "." + ext;
	}

	std::string env_filename(const std::string &name, const std::string &ext)
	{
		return "./calibs/" + env + "/" + name + "." + ext;
	}

	std::string camera_filename(const std::string &name, const std::string &ext)
	{
		return "./calibs/" + name + "." + ext;
	}

	void load_camera()
	{
		system(("v4l2ctrl -l calibs/" + camera + ".v4l2").c_str()); // load camera config

		cv::FileStorage fs(calib_filename(camera), cv::FileStorage::READ);
		fs["hfov"] >> hfov;
		fs["vfov"] >> vfov;
		fs["h"] >> h;
		fs["alpha"] >> alpha;
	}

	void save_camera()
	{
		cv::FileStorage fs(calib_filename(camera), cv::FileStorage::WRITE);
		fs << "hfov" << hfov;
		fs << "vfov" << vfov;
		fs << "h" << h;
		fs << "alpha" << alpha;
	}

	multi_camera load_multi_camera()
	{
		multi_camera cams;
		cams.push_back(::camera(2));
		cams.push_back(::camera(1));
		return cams;
	}
}
