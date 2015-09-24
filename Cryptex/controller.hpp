#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <string>
#include <utility>

class controller
{
	public:
		typedef std::pair<std::string, std::string> recv_t;

		controller();
		virtual ~controller();

		virtual void send(const std::string &cmd) = 0;
		virtual void send(const std::string &cmd, const int &val) = 0;

		virtual recv_t send_recv(const std::string &cmd) = 0;
		virtual recv_t send_recv(const std::string &cmd, const int &val) = 0;

    protected:
		static recv_t parse_recv(const std::string &line);
};

#endif // CONTROLLER_H
