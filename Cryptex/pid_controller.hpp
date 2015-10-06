#ifndef PID_CONTROLLER_H
#define PID_CONTROLLER_H

#include "error_controller.hpp"
#include <chrono>

class pid_controller : public error_controller
{
	public:
		pid_controller();
		virtual ~pid_controller();

		virtual void reset();
		virtual double step(double error);

		double Kp, Ki, Kd;

	private:
		using clock = std::chrono::high_resolution_clock;
		using seconds = std::chrono::duration<double, std::chrono::seconds::period>;
		clock::time_point prev_t;

		double prev_error;
		double integral;
};

#endif // PID_CONTROLLER_H
