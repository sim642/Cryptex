#ifndef PSMOVE_H
#define PSMOVE_H

#include <psmoveapi/psmove.h>
#include <opencv2/core/affine.hpp>

class psmove_library
{
	public:
		static psmove_library& get();

	private:
		psmove_library();

		// prevent copies
		psmove_library(const psmove_library&) = delete;
		void operator=(const psmove_library&) = delete;
};

class psmove
{
	public:
		psmove();
		virtual ~psmove();

		static int connected_count();

		PSMove_Battery_Level battery();

		bool poll();
		bool button(PSMove_Button btn);
		bool pressed(PSMove_Button btn);
		bool released(PSMove_Button btn);

		int trigger();

		bool orientation(bool enabled);
		cv::Vec3f orientation();

		void leds(cv::Scalar_<int> color);
		void rumble(int power);

	private:
		PSMove *move;
		int prev_seq;

		unsigned buttons, press, release;
};

#endif // PSMOVE_H
