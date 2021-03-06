#include "main_controller.hpp"
#include "global.hpp"
#include "logger.hpp"
#include "math.hpp"
#include <string>
#include <stdexcept>

using namespace std;

void assume_coilgun()
{
	if (!global::coilgun)
		throw runtime_error("attempt to use disabled coilun");
}

main_controller::main_controller(device_controller *new_controller) : controller(new_controller)
{
	controller->send("ir", 0); // disable interrupt replies
}

main_controller::~main_controller()
{
	dribbler(0);
}

void main_controller::kick_override(const bool& state)
{
	assume_coilgun();
	LOG("main", "kick override", state);
	controller->send("ko", state);
}

void main_controller::charge_override(const bool &state)
{
	assume_coilgun();
	LOG("main", "charge override", state);
	controller->send("co", state);
}

void main_controller::kick()
{
	assume_coilgun();
	LOG("main", "kick");
	controller->send("k");
}

void main_controller::kick(const int &us)
{
	assume_coilgun();
	LOG("main", "kick", us);
	controller->send("k", us); // MainMod takes us
}

void main_controller::kick(const double &ms)
{
	kick(static_cast<int>(ms * 1000));
}

void main_controller::charge()
{
	assume_coilgun();
	LOG("main", "charge");
	controller->send("c");
}

bool main_controller::ball()
{
	return stoi(controller->send_recv("bl", "bl").second);
}

bool main_controller::button(const int &num)
{
	string reply = "b";
	reply += to_string(num);
	return stoi(controller->send_recv("b", reply, num).second);
}

void main_controller::dribbler(const int& speed)
{
	if (global::dribbler)
		controller->send("dm", clamp(speed, {0, 255}));
}

bool main_controller::io(const int &num)
{
	string reply = "io";
	reply += to_string(num);
	return stoi(controller->send_recv("io", reply, num).second);
}

void main_controller::ping()
{
	controller->send("p");
}
