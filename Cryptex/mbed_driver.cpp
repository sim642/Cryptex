#include "mbed_driver.hpp"
#include "math.hpp"

using namespace std;

mbed_driver::mbed_driver(mbed_main_controller &new_mbed_main) : mbed_main(new_mbed_main)
{
	angles.push_back(-45); // 0
	angles.push_back(-90 - 45); // 1
	angles.push_back(90 + 45); // 2
	angles.push_back(45); // 3
}

mbed_driver::~mbed_driver()
{
	stop();
}

void mbed_driver::straight(const int &speed)
{
	omni(speed);
}

void mbed_driver::rotate(const int &speed)
{
	mbed_main.stream << "sd";
	for (size_t i = 0; i < angles.size(); i++)
	{
		if (i > 0)
			mbed_main.stream << ":";
		mbed_main.stream << -speed;
	}
	mbed_main.stream << "\n" << flush;
}

void mbed_driver::omni(const int &speed, const int &angle, const int &rot)
{
	mbed_main.stream << "sd";
	for (size_t i = 0; i < angles.size(); i++)
	{
		if (i > 0)
			mbed_main.stream << ":";
		mbed_main.stream << -(speed * -sin(deg2rad(angles[i] - angle)) + rot);
	}
	mbed_main.stream << "\n" << flush;
}

void mbed_driver::stop()
{
	rotate(0);
}
