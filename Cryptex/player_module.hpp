#ifndef PLAYER_MODULE_H
#define PLAYER_MODULE_H

#include "module.hpp"

#include "pid_controller.hpp"
#include <chrono>

class player_module : public module
{
	public:
		player_module();
		virtual ~player_module();

		virtual type run(const type &prev_module);

	private:
		enum state_t
		{
			Undefined,
			Start,
			Manual,
			Ball,
			BallGrab,
			GoalFind,
			Goal
		};

		enum class half
		{
			left,
			right
		};

		const int btn_team = 1;
		const double scalelow = 0.5;
		const int dribblerspeed = 255;
		const bool coilenabled = true;
		const int kicktime = 3;
		const int kickstotal = 2;

		void set_state(const state_t &new_state);
		void reset_statestart();
		float get_statestart();

		state_t state;
		std::chrono::high_resolution_clock::time_point statestart;

		pid_controller speed_controller, rotate_controller;
};

#endif // PLAYER_MODULE_H
