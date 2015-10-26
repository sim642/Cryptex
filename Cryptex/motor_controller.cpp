#include "motor_controller.hpp"

motor_controller::motor_controller(device_controller *new_controller) : controller(new_controller)
{
	controller->send("dr", 0); // set motor polarity
}

motor_controller::~motor_controller()
{

}

void motor_controller::drive(const int &speed)
{
	controller->send("sd", speed);
}

int motor_controller::encoder()
{
	return stoi(controller->send_recv("e", "e").second);
}

void motor_controller::reset_encoder()
{
	controller->send("re");
}
