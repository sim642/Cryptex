#ifndef MBED_MAIN_CONTROLLER_H
#define MBED_MAIN_CONTROLLER_H

#include "serial_device.hpp"

class mbed_driver;

class mbed_main_controller
{
	public:
		mbed_main_controller(boost::asio::io_service &io, const std::string &dev);
		virtual ~mbed_main_controller();

		void kick_override(const bool &state);
		void charge_override(const bool &state);
		void kick();
		void kick(const int &us);
		void kick(const double &ms);
		void charge();

		bool ball();

		bool button(const int &num);

		void dribbler(const int &speed);

		bool io(const int &num);

		void ping();

		void failsafe(const bool &state);

		friend class mbed_driver;

	private:
		void assume_coilgun();

		boost::asio::serial_port port;
		serial_stream stream;
};

#endif // MBED_MAIN_CONTROLLER_H
