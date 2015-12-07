#ifndef LINECALIBRATOR_MODULE_H
#define LINECALIBRATOR_MODULE_H

#include "module.hpp"

class linecalibrator_module : public module
{
	public:
		linecalibrator_module();
		virtual ~linecalibrator_module();

		virtual type run(const type &prev_module);
};

#endif // LINECALIBRATOR_MODULE_H
