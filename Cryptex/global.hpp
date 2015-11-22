#ifndef GLOBAL_H
#define GLOBAL_H

#include <string>

namespace global
{
	extern std::string env;
	extern int video_id;
	extern char field, id;
	extern bool coilgun;
	extern bool referee;

	extern float hfov, vfov, h, alpha;

	std::string calib_filename(const std::string &name, const std::string &ext = "yml");
	std::string env_filename(const std::string &name, const std::string &ext = "yml");
}

#endif // GLOBAL_H
