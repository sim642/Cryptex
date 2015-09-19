#ifndef DRIVER_H
#define DRIVER_H

#include "serial_scanner.hpp"
#include "motor.hpp"
#include <vector>

class driver
{
	public:
		driver(serial_scanner &scanner);
		virtual ~driver();

		void straight(const int &speed);
		void rotate(const int &speed);
		void omni(const int &speed, const int &angle);
		void stop();

	private:
		std::vector<motor> motors;
		std::vector<int> angles;
};

#endif // DRIVER_H
