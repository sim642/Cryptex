#ifndef MOTOR_H
#define MOTOR_H

#include "serial_controller.hpp"

class motor
{
	public:
		motor(serial_controller &controler);
		virtual ~motor();

		void drive(const int &speed);

	private:
		serial_controller &controller;
};

#endif // MOTOR_H
