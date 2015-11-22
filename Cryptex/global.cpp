#include "global.hpp"

namespace global
{
	std::string env;
	int video_id;
	char field, id;
	bool coilgun, referee;
	float hfov, vfov, h, alpha;

	std::string calib_filename(const std::string &name, const std::string &ext)
	{
		return "./calibs/" + name + "." + ext;
	}

	std::string env_filename(const std::string &name, const std::string &ext)
	{
		return "./calibs/" + env + "/" + name + "." + ext;
	}
}
