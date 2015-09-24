#ifndef MOTOR_H
#define MOTOR_H

#include "controller.hpp"

class motor
{
	public:
		motor(controller &controler);
		virtual ~motor();

		void drive(const int &speed);

	private:
		controller &control;
};

#endif // MOTOR_H
