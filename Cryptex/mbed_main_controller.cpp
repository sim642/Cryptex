#include "mbed_main_controller.hpp"
#include "global.hpp"
#include "logger.hpp"
#include "math.hpp"
#include <string>
#include <stdexcept>

using namespace std;

void mbed_main_controller::assume_coilgun()
{
	if (!global::coilgun)
		throw runtime_error("attempt to use disabled coilun");
}

mbed_main_controller::mbed_main_controller(boost::asio::io_service &io, const std::string &dev) : port(io, dev), stream(port)
{
	port.set_option(boost::asio::serial_port_base::baud_rate(9600));
}

mbed_main_controller::~mbed_main_controller()
{
	dribbler(0);
}

void mbed_main_controller::kick_override(const bool& state)
{
	assume_coilgun();
	LOG("main", "kick override", state);
	//controller->send("ko", state);
}

void mbed_main_controller::charge_override(const bool &state)
{
	assume_coilgun();
	LOG("main", "charge override", state);
	//controller->send("co", state);
}

void mbed_main_controller::kick()
{
	assume_coilgun();
	LOG("main", "kick");
	//controller->send("k");
}

void mbed_main_controller::kick(const int &us)
{
	assume_coilgun();
	LOG("main", "kick", us);
	//controller->send("k", us); // MainMod takes us
}

void mbed_main_controller::kick(const double &ms)
{
	kick(static_cast<int>(ms * 1000));
}

void mbed_main_controller::charge()
{
	assume_coilgun();
	LOG("main", "charge");
	//controller->send("c");
}

bool mbed_main_controller::ball()
{
	//return stoi(controller->send_recv("bl", "bl").second);
	return false;
}

bool mbed_main_controller::button(const int &num)
{
	/*string reply = "b";
	reply += to_string(num);
	return stoi(controller->send_recv("b", reply, num).second);*/
	return false;
}

void mbed_main_controller::dribbler(const int& speed)
{
	/*if (global::dribbler)
		controller->send("dm", clamp(speed, {0, 255}));*/
}

bool mbed_main_controller::io(const int &num)
{
	/*string reply = "io";
	reply += to_string(num);
	return stoi(controller->send_recv("io", reply, num).second);*/
	return false;
}

void mbed_main_controller::ping()
{
	//controller->send("p");
}
