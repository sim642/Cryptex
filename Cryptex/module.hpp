#ifndef MODULE_H
#define MODULE_H

class module
{
	public:
		enum class type
		{
			exit = -1,
			menu = 0,
			config,
			calibrator,
			player,
			test,
			psmove
		};

		module();
		virtual ~module();

		virtual type run(const type &prev_module) = 0;
};

#endif // MODULE_H
