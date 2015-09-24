#ifndef RS485_CONTROLLER_H
#define RS485_CONTROLLER_H

#include "controller.hpp"
#include <string>
#include <utility>

class rs485_dongle;

class rs485_controller : public controller
{
	public:
		rs485_controller(rs485_dongle *new_dongle, const int &new_id);
		virtual ~rs485_controller();

		virtual void send(const std::string &cmd);
		virtual void send(const std::string &cmd, const int &val);

		virtual recv_t send_recv(const std::string &cmd);
		virtual recv_t send_recv(const std::string &cmd, const int &val);

	private:
		rs485_dongle *dongle;
		const int id;
};

#endif // RS485_CONTROLLER_H
