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

void rs485_controller::send(const std::string &cmd, const int &val)
{
	dongle->send(id, cmd, val);
}

rs485_controller::recv_t rs485_controller::send_recv(const std::string &cmd)
{
	return dongle->send_recv(id, cmd);
}

rs485_controller::recv_t rs485_controller::send_recv(const std::string &cmd, const int &val)
{
	return dongle->send_recv(id, cmd, val);
}
