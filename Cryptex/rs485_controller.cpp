#include "rs485_controller.hpp"
#include "rs485_dongle.hpp"

rs485_controller::rs485_controller(rs485_dongle *new_dongle, const int &new_id) : dongle(new_dongle), id(new_id)
{

}

rs485_controller::~rs485_controller()
{

}


void rs485_controller::send(const std::string &cmd)
{
	dongle->send(id, cmd);
}

void rs485_controller::send(const std::string &cmd, const std::string &val)
{
	dongle->send(id, cmd, val);
}

std::string rs485_controller::read_line()
{
	return dongle->read_line(id);
}
