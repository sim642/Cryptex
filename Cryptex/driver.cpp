#include "driver.hpp"
#include "device_id.hpp"
#include "math.hpp"

driver::driver(device_manager &manager)
{
	motors.emplace_back(manager[device_id::motor_left]);
	angles.push_back(60);
	motors.emplace_back(manager[device_id::motor_back]);
	angles.push_back(180);
	motors.emplace_back(manager[device_id::motor_right]);
	angles.push_back(-60);
}

driver::~driver()
{
	stop();
}

void driver::straight(const int &speed)
{
	motors[0].drive(-speed);
	motors[1].drive(0);
	motors[2].drive(speed);
}

void driver::rotate(const int &speed)
{
	for (size_t i = 0; i < motors.size(); i++)
		motors[i].drive(speed);
}

void driver::omni(const int &speed, const int &angle, const int &rot)
{
	for (size_t i = 0; i < motors.size(); i++)
		motors[i].drive(speed * -sin(deg2rad(angles[i] - angle)) + rot);
}

void driver::stop()
{
	rotate(0);
}
