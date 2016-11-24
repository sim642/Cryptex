#ifndef GLOBAL_H
#define GLOBAL_H

#include <string>
#include "camera.hpp"

namespace global
{
	extern std::string env;
	extern int video_id;
	extern char field, id;
	extern bool dribbler, coilgun;
	extern bool referee;

	extern std::string camera;
	extern float hfov, vfov, h, alpha;

	std::string calib_filename(const std::string &name, const std::string &ext = "yml");
	std::string env_filename(const std::string &name, const std::string &ext = "yml");
	std::string camera_filename(const std::string &name, const std::string &ext = "yml");

	void load_camera();
	void save_camera();
}

#endif // GLOBAL_H
