#include "driver.hpp"
#include "serial_id.hpp"

driver::driver(serial_prober &prober)
{
	motors.emplace_back(*prober[motor_left]);
	motors.emplace_back(*prober[motor_back]);
	motors.emplace_back(*prober[motor_right]);
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
