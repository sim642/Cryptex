#ifndef CONFIG_MODULE_H
#define CONFIG_MODULE_H

#include "module.hpp"
#include <string>
#include <tuple>
#include <vector>

class config_module : public module
{
	public:
		config_module();
		virtual ~config_module();

		virtual type run(const type &prev_module);

	private:
		enum class var_type
		{
			string_t,
			char_t,
			int_t,
			bool_t,
			float_t
		};
		typedef std::tuple<std::string, var_type, void*> choice_t;

		std::string get_var(const var_type &type, void *ptr);
		void set_var(const var_type &type, void *ptr, const std::string &val);
		void print_choices();

		std::vector<choice_t> choices;
};

#endif // CONFIG_MODULE_H
