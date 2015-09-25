#ifndef DEVICE_CONTROLLER_H
#define DEVICE_CONTROLLER_H

#include <string>
#include <utility>

class device_controller
{
	public:
		typedef std::pair<std::string, std::string> recv_t;

		device_controller();
		virtual ~device_controller();

		virtual void send(const std::string &cmd) = 0;
		virtual void send(const std::string &cmd, const int &val) = 0;

		virtual recv_t send_recv(const std::string &cmd) = 0;
		virtual recv_t send_recv(const std::string &cmd, const int &val) = 0;

    protected:
		static recv_t parse_recv(const std::string &line);
};

#endif // DEVICE_CONTROLLER_H
