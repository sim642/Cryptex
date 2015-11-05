#include "psmove.hpp"
#include <stdexcept>

using namespace std;

psmove_library& psmove_library::get()
{
	static psmove_library lib;
	return lib;
}

psmove_library::psmove_library()
{
	if (!psmove_init(PSMOVE_CURRENT_VERSION))
		throw runtime_error("PSMove API init failed");
}

psmove::psmove()
{
	psmove_library::get(); // guarantee library initialization

	move = psmove_connect();
	if (move == NULL)
		throw runtime_error("PSMove controller not found");

	prev_seq = 0;
}

psmove::~psmove()
{
	psmove_set_leds(move, 0, 0, 0);
	psmove_set_rumble(move, 0);
	psmove_update_leds(move);

	psmove_disconnect(move);
}

int psmove::connected_count()
{
	psmove_library::get(); // guarantee library initialization

	return psmove_count_connected();
}

PSMove_Battery_Level psmove::battery()
{
	return psmove_get_battery(move);
}

bool psmove::poll()
{
	int seq = psmove_poll(move);

	buttons = psmove_get_buttons(move);
	psmove_get_button_events(move, &press, &release);

	if ((prev_seq > 0) && ((prev_seq % 16) != (seq - 1)))
	{
		prev_seq = seq;
		return false;
	}

	prev_seq = seq;
	return true;
}

bool psmove::button(PSMove_Button btn)
{
	return buttons & btn;
}

bool psmove::pressed(PSMove_Button btn)
{
	return press & btn;
}

bool psmove::released(PSMove_Button btn)
{
	return release & btn;
}

int psmove::trigger()
{
	return psmove_get_trigger(move);
}

bool psmove::orientation(bool enabled)
{
	psmove_enable_orientation(move, static_cast<PSMove_Bool>(enabled));
	return psmove_has_orientation(move);
}

cv::Vec3f psmove::orientation()
{
	float w, x, y, z;
	psmove_get_orientation(move, &w, &x, &y, &z);

	float phi = atan2(x*z+y*w, x*w-y*z);
	float the = acos(-x*x-y*y+z*z+w*w);
	float psi = atan2(x*z-y*w, y*z+x*w);

	return cv::Vec3f(phi, the, psi);
}

void psmove::leds(cv::Scalar_<int> color)
{
	psmove_set_leds(move, color[0], color[1], color[2]);
	psmove_update_leds(move);
}

void psmove::rumble(int power)
{
	psmove_set_rumble(move, power);
	psmove_update_leds(move);
}
