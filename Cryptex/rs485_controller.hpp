#ifndef RS485_CONTROLLER_H
#define RS485_CONTROLLER_H

#include <string>
#include <utility>

class rs485_dongle;

class rs485_controller
{
	public:
		typedef std::pair<std::string, std::string> recv_t;

		rs485_controller(rs485_dongle *new_dongle, const int &new_id);
		virtual ~rs485_controller();

		void send(const std::string &cmd);
		void send(const std::string &cmd, const int &val);

		recv_t send_recv(const std::string &cmd);
		recv_t send_recv(const std::string &cmd, const int &val);

	private:
		rs485_dongle *dongle;
		const int id;
};

#endif // RS485_CONTROLLER_H
