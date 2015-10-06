#include "pid_controller.hpp"

pid_controller::pid_controller() : Kp(0), Ki(0), Kd(0)
{
	reset();
}

pid_controller::~pid_controller()
{

}

void pid_controller::reset()
{
	prev_t = clock::now();
	prev_error = 0;
	integral = 0;
}

double pid_controller::step(double error)
{
	clock::time_point t = clock::now();
	double dt = std::chrono::duration_cast<seconds>(t - prev_t).count();

	integral += error * dt;
	double derivative = (error - prev_error) / dt;
	double out = Kp * error + Ki * integral + Kd * derivative;

	prev_error = error;
	prev_t = t;

	return out;
}
