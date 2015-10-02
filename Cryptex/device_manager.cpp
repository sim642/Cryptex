#include "device_manager.hpp"

device_manager::device_manager()
{
	for (auto &ptr : controllers)
		ptr = nullptr;
}

device_manager::~device_manager()
{
	for (auto &ptr : controllers)
		if (ptr)
			delete ptr;
}

device_controller* device_manager::operator[] (const int &id)
{
	if (!controllers[id])
		controllers[id] = request(id);

	return controllers[id];
}

device_controller* device_manager::operator[] (const device_id& dev_id)
{
	return operator[](static_cast<int>(dev_id));
}

device_controller* device_manager::request(const int &id)
{
	return nullptr;
}
