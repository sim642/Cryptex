#ifndef DRIVER_H
#define DRIVER_H

#include "serial_prober.hpp"
#include "motor.hpp"
#include <vector>

class driver
{
	public:
		driver(serial_prober &prober);
		virtual ~driver();

		void straight(const int &speed);
	private:
		std::vector<motor> motors;
};

#endif // DRIVER_H
