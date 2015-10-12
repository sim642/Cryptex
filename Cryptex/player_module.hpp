#ifndef PLAYER_MODULE_H
#define PLAYER_MODULE_H

#include "module.hpp"

#include "pid_controller.hpp"

class player_module : public module
{
	public:
		player_module();
		virtual ~player_module();

		virtual type run(const type &prev_module);

	private:
		enum state_t
		{
			Start,
			Manual,
			Ball,
			GoalFind,
			Goal
		};

		void set_state(const state_t &new_state);

		state_t state;


		pid_controller speed_controller, rotate_controller;
};

#endif // PLAYER_MODULE_H
