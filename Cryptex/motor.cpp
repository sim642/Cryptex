#include "motor.hpp"

motor::motor(controller &new_controller) : control(new_controller)
{
	control.send("dr", 0); // set motor polarity
}

motor::~motor()
{

}

void motor::drive(const int &speed)
{
	control.send("sd", speed);
}
