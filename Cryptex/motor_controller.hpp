#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H

#include "device_controller.hpp"

class motor_controller
{
	public:
		motor_controller(device_controller *new_controller);
		virtual ~motor_controller();

		void drive(const int &speed);

		int encoder();
		void reset_encoder();

	private:
		device_controller *controller;
};

#endif // MOTOR_CONTROLLER_H
