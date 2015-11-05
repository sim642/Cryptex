#include "referee_controller.hpp"
#include "global.hpp"
#include <tuple>

using namespace std;

referee_controller::referee_controller(srf_dongle &new_dongle) : dongle(new_dongle)
{

}

referee_controller::~referee_controller()
{

}

referee_controller::poll_t referee_controller::poll()
{
	auto srf_data = dongle.recv_parsed();
	if (get<0>(srf_data))
	{
		char field, target;
		string cmd;
		tie(field, target, cmd) = srf_data;

		if (field == global::field && (target == global::id || target == 'X'))
		{
			if (cmd == "START")
			{
				return Start;
			}
			else if (cmd == "STOP")
			{
				return Stop;
			}

			if (target == global::id)
			{
				dongle.send(field, target, "ACK");
			}
		}
	}

	return None;
}
