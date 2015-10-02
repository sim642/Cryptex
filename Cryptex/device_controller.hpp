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

		virtual recv_t recv(const std::string &reply);

		virtual recv_t send_recv(const std::string &cmd, const std::string &reply);
		virtual recv_t send_recv(const std::string &cmd, const std::string &reply, const int &val);

    protected:
		virtual std::string read_line() = 0;
		static recv_t parse_recv(const std::string &line);
};

#endif // DEVICE_CONTROLLER_H
