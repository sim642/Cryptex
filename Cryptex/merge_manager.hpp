#ifndef MERGE_MANAGER_H
#define MERGE_MANAGER_H

#include "device_manager.hpp"
#include <vector>

class merge_manager : public device_manager
{
	public:
		merge_manager();
		virtual ~merge_manager();

		void add_manager(device_manager *manager);

	private:
		virtual device_controller* request(const int &id);

		std::vector<device_manager*> managers;
};

#endif // MERGE_MANAGER_H
