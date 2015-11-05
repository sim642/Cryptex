#ifndef REFEREE_CONTROLLER_H
#define REFEREE_CONTROLLER_H

#include "srf_dongle.hpp"

class referee_controller
{
	public:
		enum poll_t
		{
			None,
			Start,
			Stop
		};

		referee_controller(srf_dongle &new_dongle);
		virtual ~referee_controller();

		poll_t poll();

	private:
		srf_dongle &dongle;
};

#endif // REFEREE_CONTROLLER_H
