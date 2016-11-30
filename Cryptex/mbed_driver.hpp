#ifndef MBED_DRIVER_H
#define MBED_DRIVER_H

#include "mbed_main_controller.hpp"
#include <vector>

class mbed_driver
{
	public:
		mbed_driver(mbed_main_controller &new_mbed_main);
		virtual ~mbed_driver();

		void straight(const int &speed);
		void rotate(const int &speed);
		void omni(const int &speed, const int &angle = 0, const int &rot = 0);
		void stop();

	private:
		mbed_main_controller &mbed_main;
		std::vector<int> angles;
};

#endif // MBED_DRIVER_H
