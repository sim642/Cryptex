#ifndef DEVICE_MANAGER_H
#define DEVICE_MANAGER_H

#include "device_controller.hpp"
#include <array>

class device_manager
{
	public:
		device_manager();
		virtual ~device_manager();

		virtual device_controller* operator[] (const int &id);

	protected:
		virtual device_controller* request(const int &id);

		std::array<device_controller*, 256> controllers;
};

#endif // DEVICE_MANAGER_H
