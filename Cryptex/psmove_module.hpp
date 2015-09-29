#ifndef PSMOVE_MODULE_H
#define PSMOVE_MODULE_H

#include "module.hpp"

class psmove_module : public module
{
	public:
		psmove_module();
		virtual ~psmove_module();

		virtual type run(const type &prev_module);
};

#endif // PSMOVE_MODULE_H
