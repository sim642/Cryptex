#ifndef TEST_MODULE_H
#define TEST_MODULE_H

#include "module.hpp"

class test_module : public module
{
	public:
		test_module();
		virtual ~test_module();

		virtual type run(const type &prev_module);
};

#endif // TEST_MODULE_H
