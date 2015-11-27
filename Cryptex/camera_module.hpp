#ifndef CAMERA_MODULE_H
#define CAMERA_MODULE_H

#include "module.hpp"

class camera_module : public module
{
	public:
		camera_module();
		virtual ~camera_module();

		virtual type run(const type &prev_module);
};

#endif // CAMERA_MODULE_H
