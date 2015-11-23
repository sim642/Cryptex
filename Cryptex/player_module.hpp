#ifndef PLAYER_MODULE_H
#define PLAYER_MODULE_H

#include "module.hpp"

#include "pid_controller.hpp"
#include <string>
#include <vector>
#include <chrono>
#include <map>
#include <functional>

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
			BallFind,
			BallDrive,
			BallGrab,
			GoalFind,
			GoalAim,
			GoalDrive
		};
		static const std::map<state_t, std::string> state_name;

		enum class half
		{
			left,
			right
		};

		const int btn_team = 1;
		const int dribblerspeed = 255;
		//const std::vector<int> kicks = {2500, 8000};
		const std::vector<int> kicks = {4000};

		typedef std::function<void(state_t)> transition_t;
		std::map<state_t, transition_t> transitions;
		void set_state(const state_t &new_state, const std::string &changer = "");
		void reset_statestart();
		float get_statestart();

		state_t state;
		std::chrono::high_resolution_clock::time_point statestart;

		pid_controller speed_controller, rotate_controller;
};

#endif // PLAYER_MODULE_H
