#ifndef PLAYER_MODULE_H
#define PLAYER_MODULE_H

#include "module.hpp"

class player_module : public module
{
	public:
		player_module();
		virtual ~player_module();

		virtual type run(const type &prev_module);
};

#endif // PLAYER_MODULE_H
