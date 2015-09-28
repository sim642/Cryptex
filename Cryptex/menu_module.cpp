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
	cout << "asd" << endl;

	return module::type::exit;
}
