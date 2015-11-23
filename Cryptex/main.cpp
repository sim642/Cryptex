#include "global.hpp"
#include "module.hpp"
#include <map>
#include <stdexcept>
#include <cstdlib>
#include "menu_module.hpp"
#include "config_module.hpp"
#include "calibrator_module.hpp"
#include "player_module.hpp"
#include "test_module.hpp"
#include "psmove_module.hpp"
#include "camera_module.hpp"

using namespace std;

int main()
{
	global::env = "test";
	global::video_id = 0;
	global::field = 'B';
	global::id = 'D';
	global::coilgun = true;
	global::referee = false;
	global::hfov = 75;
	global::vfov = 56;
	global::h = 0.23;
	global::alpha = 90 - 23;

	system("v4l2ctrl -l calibs/pseye.v4l2"); // load camera config

	map<module::type, module*> modules;
	modules[module::type::menu] = new menu_module();
	modules[module::type::config] = new config_module();
	modules[module::type::calibrator] = new calibrator_module();
	modules[module::type::player] = new player_module();
	modules[module::type::test] = new test_module();
	modules[module::type::psmove] = new psmove_module();
	modules[module::type::camera] = new camera_module();

	module::type next_module = module::type::menu;
	module::type prev_module = module::type::exit;

	while (next_module != module::type::exit)
	{
		auto cur_module = next_module;
		auto it = modules.find(cur_module);
		if (it == modules.end() || !it->second) // module not found or initialized
			throw domain_error("module not initialized");

		next_module = (it->second)->run(prev_module);
		prev_module = cur_module;
	}

	for (auto &p : modules)
		delete p.second;

	return EXIT_SUCCESS;
}
