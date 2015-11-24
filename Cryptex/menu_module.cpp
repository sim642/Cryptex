#include "menu_module.hpp"
#include <iostream>

using namespace std;

menu_module::menu_module()
{

}

menu_module::~menu_module()
{

}

module::type menu_module::run(const module::type &prev_module)
{
	while (1)
	{
		cout << "1. calibrator" << endl;
		cout << "2. config" << endl;
		cout << "3. player" << endl;
		cout << "4. test" << endl;
		cout << "5. psmove" << endl;
		cout << "6. camera" << endl;
		cout << "7. exit" << endl;

		cout << "> ";
		int selection;
		cin >> selection;

		switch (selection)
		{
			case 1:
				return module::type::calibrator;

			case 2:
				return module::type::config;

			case 3:
				return module::type::player;

			case 4:
				return module::type::test;

			case 5:
				return module::type::psmove;

			case 6:
				return module::type::camera;

			case 7:
				return module::type::exit;

			default:
				cout << ">> invalid selection" << endl;
		}
	}
}
