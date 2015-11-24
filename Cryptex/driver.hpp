#ifndef DRIVER_H
#define DRIVER_H

#include "device_manager.hpp"
#include "motor_controller.hpp"
#include <vector>
#include <utility>

class driver
{
	public:
		driver(device_manager &manager);
		virtual ~driver();

		void straight(const int &speed);
		void rotate(const int &speed);
		void omni(const int &speed, const int &angle = 0, const int &rot = 0);
		void stop();

	private:
		std::vector<std::pair<motor_controller, int>> motors;
};

#endif // DRIVER_H
