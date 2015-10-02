#ifndef MOTOR_H
#define MOTOR_H

#include "device_controller.hpp"

class motor
{
	public:
		motor(device_controller *new_controller);
		virtual ~motor();

		void drive(const int &speed);

		int encoder();
		void reset_encoder();

	private:
		device_controller *controller;
};

#endif // MOTOR_H
