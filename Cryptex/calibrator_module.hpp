#ifndef CALIBRATOR_MODULE_H
#define CALIBRATOR_MODULE_H

#include "module.hpp"

class calibrator_module : public module
{
	public:
		calibrator_module();
		virtual ~calibrator_module();

		virtual type run(const type &prev_module);
};

#endif // CALIBRATOR_MODULE_H
