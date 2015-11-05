#include "merge_manager.hpp"

merge_manager::merge_manager()
{

}

merge_manager::~merge_manager()
{
	for (auto &manager : managers)
		delete manager;
}

void merge_manager::add_manager(device_manager* manager)
{
	managers.push_back(manager);
}

device_controller* merge_manager::request(const int& id)
{
	for (auto &manager : managers)
	{
		device_controller *controller = (*manager)[id];
		if (controller)
			return controller;
	}

	return nullptr;
}
