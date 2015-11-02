#ifndef DRIVER_H
#define DRIVER_H

#include "device_manager.hpp"
#include "motor_controller.hpp"
#include <vector>

class driver
{
	public:
		driver(device_manager &manager);
		virtual ~driver();

		void straight(const int &speed);
		void rotate(const int &speed);
		void omni(const int &speed, const int &angle, const int &rot = 0);
		void stop();

	private:
		std::vector<motor_controller> motors;
		std::vector<int> angles;
};

#endif // DRIVER_H
