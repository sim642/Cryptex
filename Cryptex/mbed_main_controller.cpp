#include "mbed_main_controller.hpp"
#include "global.hpp"
#include "logger.hpp"
#include "math.hpp"
#include <string>
#include <stdexcept>
#include <thread>

using namespace std;

void mbed_main_controller::assume_coilgun()
{
	if (!global::coilgun)
		throw runtime_error("attempt to use disabled coilun");
}

mbed_main_controller::mbed_main_controller(boost::asio::io_service &io, const std::string &dev) : port(io, dev), stream(port)
{
	port.set_option(boost::asio::serial_port_base::baud_rate(115200));
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
	stream << "c" << (state ? 1 : 0) << "\n" << flush;
}

void mbed_main_controller::kick()
{
	assume_coilgun();
	LOG("main", "kick");
	stream << "k\n" << flush;
}

void mbed_main_controller::kick(const int &us)
{
	assume_coilgun();
	LOG("main", "kick", us);
	stream << "k" << us << "\n" << flush;
}

void mbed_main_controller::kick(const double &ms)
{
	kick(static_cast<int>(ms * 1000));
}

void mbed_main_controller::charge()
{
	assume_coilgun();
	LOG("main", "charge");
	stream << "c1\n" << flush;
	this_thread::sleep_for(chrono::milliseconds(1000));
	stream << "c0\n" << flush;
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
	if (global::dribbler)
		//controller->send("dr", clamp(speed, {0, 255}));
		stream << "d" << (speed > 0 ? 1 : 0) << "\n" << flush;
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
