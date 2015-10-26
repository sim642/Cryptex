#ifndef MAIN_CONTROLLER_H
#define MAIN_CONTROLLER_H

#include "device_controller.hpp"

class main_controller
{
	public:
		main_controller(device_controller *new_controller);
		virtual ~main_controller();

		void kick_override(const bool &state);
		void charge_override(const bool &state);
		void kick();
		void charge();

		bool ball();

		bool button(const int &num);

		void dribbler(const int &speed);

	private:
		device_controller *controller;
};

#endif // MAIN_CONTROLLER_H
