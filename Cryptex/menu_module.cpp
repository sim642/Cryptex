#include "menu_module.hpp"
#include <iostream>
#include <limits>

using namespace std;

menu_module::menu_module()
{
	modules.push_back(make_pair("config", module::type::config));
	modules.push_back(make_pair("calibrator", module::type::calibrator));
	modules.push_back(make_pair("linecalibrator", module::type::linecalibrator));
	modules.push_back(make_pair("camera", module::type::camera));
	modules.push_back(make_pair("player", module::type::player));
	modules.push_back(make_pair("test", module::type::test));
	modules.push_back(make_pair("psmove", module::type::psmove));
	modules.push_back(make_pair("exit", module::type::exit));
}

menu_module::~menu_module()
{

}

module::type menu_module::run(const module::type &prev_module)
{
	while (1)
	{
		for (unsigned int i = 0; i < modules.size(); i++)
		{
			cout << (i + 1) << ". " << modules[i].first << endl;
		}

		cout << "> ";
		unsigned int selection;
		if ((cin >> selection) && (selection >= 1 && selection <= modules.size()))
			return modules[selection - 1].second;
		else
			cout << ">> invalid selection" << endl;
	}
}
