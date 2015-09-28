#include "global.hpp"
#include "module.hpp"
#include <map>
#include <stdexcept>
#include "menu_module.hpp"
#include "calibrator_module.hpp"
#include "player_module.hpp"
#include "test_module.hpp"

using namespace std;

int main()
{
	global::env = "test";
	global::video_id = 0;

	map<module::type, module*> modules;
	modules[module::type::menu] = new menu_module();
	modules[module::type::calibrator] = new calibrator_module();
	modules[module::type::player] = new player_module();
	modules[module::type::test] = new test_module();

	module::type next_module = module::type::menu;
	module::type prev_module = module::type::exit;

	while (next_module != module::type::exit)
	{
		auto cur_module = next_module;
		auto it = modules.find(cur_module);
		if (it == modules.end()) // module not found
			throw domain_error("module not initialized");

		next_module = (it->second)->run(prev_module);
		prev_module = cur_module;
	}

	for (auto &p : modules)
		delete p.second;

	return EXIT_SUCCESS;
}
