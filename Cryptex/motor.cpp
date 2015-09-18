#include "motor.hpp"

motor::motor(serial_controller &new_controller) : controller(new_controller)
{
	controller.send("dr", 0); // set motor polarity
}

motor::~motor()
{

}

void motor::drive(const int &speed)
{
	controller.send("sd", speed);
}
