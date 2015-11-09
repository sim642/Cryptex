#include "main_controller.hpp"
#include <string>

using namespace std;

main_controller::main_controller(device_controller *new_controller) : controller(new_controller)
{
	controller->send("ir", 0); // disable interrupt replies
}

main_controller::~main_controller()
{

}

void main_controller::kick_override(const bool& state)
{
	controller->send("ko", state);
}

void main_controller::charge_override(const bool &state)
{
	controller->send("co", state);
}

void main_controller::kick()
{
	controller->send("k");
}

void main_controller::kick(const int &ms)
{
	controller->send("k", ms);
}

void main_controller::charge()
{
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
	controller->send("dm", speed);
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