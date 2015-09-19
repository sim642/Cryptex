#include "driver.hpp"
#include "serial_id.hpp"
#include "math.hpp"

driver::driver(serial_scanner &scanner)
{
	motors.emplace_back(*scanner[serial_id::motor_left]);
	angles.push_back(60);
	motors.emplace_back(*scanner[serial_id::motor_back]);
	angles.push_back(180);
	motors.emplace_back(*scanner[serial_id::motor_right]);
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

void driver::omni(const int &speed, const int &angle)
{
	for (size_t i = 0; i < motors.size(); i++)
		motors[i].drive(speed * -sin(deg2rad(angles[i] - angle)));
}

void driver::stop()
{
	rotate(0);
}
