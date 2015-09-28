#ifndef MENU_MODULE_H
#define MENU_MODULE_H

#include "module.hpp"

class menu_module : public module
{
	public:
		menu_module();
		virtual ~menu_module();

		virtual type run(const type &prev_module);
};

#endif // MENU_MODULE_H
