#include "global.hpp"

namespace global
{
	std::string env;

	std::string calib_filename(const std::string &name, const std::string &ext)
	{
		return "./calibs/" + name + "." + ext;
	}

	std::string env_filename(const std::string &name, const std::string &ext)
	{
		return "./calibs/" + env + "/" + name + "." + ext;
	}
}
