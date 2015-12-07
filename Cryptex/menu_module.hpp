#ifndef MENU_MODULE_H
#define MENU_MODULE_H

#include "module.hpp"
#include <string>
#include <utility>
#include <vector>

class menu_module : public module
{
	public:
		menu_module();
		virtual ~menu_module();

		virtual type run(const type &prev_module);

	private:
		std::vector<std::pair<std::string, module::type>> modules;
};

#endif // MENU_MODULE_H
