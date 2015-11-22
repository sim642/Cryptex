#include "driver.hpp"
#include "device_id.hpp"
#include "math.hpp"

using namespace std;

driver::driver(device_manager &manager)
{
	motors.push_back(make_pair(manager[device_id::motor_left], 60));
	motors.push_back(make_pair(manager[device_id::motor_left], 180));
	motors.push_back(make_pair(manager[device_id::motor_left], -60));
}

driver::~driver()
{
	stop();
}

void driver::straight(const int &speed)
{
	omni(speed);
}

void driver::rotate(const int &speed)
{
	for (auto &p : motors)
		p.first.drive(speed);
}

void driver::omni(const int &speed, const int &angle, const int &rot)
{
	for (auto &p : motors)
		p.first.drive(speed * -sin(deg2rad(p.second - angle)) + rot);
}

void driver::stop()
{
	rotate(0);
}
