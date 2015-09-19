#include "driver.hpp"
#include "serial_id.hpp"

driver::driver(serial_scanner &scanner)
{
	motors.emplace_back(*scanner[serial_id::motor_left]);
	motors.emplace_back(*scanner[serial_id::motor_back]);
	motors.emplace_back(*scanner[serial_id::motor_right]);
}

driver::~driver()
{

}

void driver::straight(const int &speed)
{
	motors[0].drive(speed);
	motors[1].drive(0);
	motors[2].drive(-speed);
}
