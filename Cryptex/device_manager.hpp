#ifndef DEVICE_MANAGER_H
#define DEVICE_MANAGER_H

#include "device_controller.hpp"

class device_manager
{
	public:
		device_manager();
		virtual ~device_manager();

		virtual device_controller* operator[] (const int &id) = 0;

	private:
};

#endif // DEVICE_MANAGER_H
