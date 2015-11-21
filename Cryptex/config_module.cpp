#include "config_module.hpp"

#include <iostream>
#include <limits>
#include "global.hpp"

using namespace std;

config_module::config_module()
{
	choices.push_back(make_tuple("env", var_type::string_t, &global::env));
	choices.push_back(make_tuple("video", var_type::int_t, &global::video_id));
	choices.push_back(make_tuple("field", var_type::char_t, &global::field));
	choices.push_back(make_tuple("id", var_type::char_t, &global::id));
	choices.push_back(make_tuple("coilgun", var_type::bool_t, &global::coilgun));
	choices.push_back(make_tuple("referee", var_type::bool_t, &global::referee));
}

config_module::~config_module()
{

}

module::type config_module::run(const module::type &prev_module)
{
	string line;
	cin.ignore(numeric_limits<streamsize>::max(), '\n');

	while (print_choices(), cout << "> ", getline(cin, line))
	{
		if (line.empty())
			break;

		int i = stoi(line) - 1;
		auto &choice = choices[i];

		cout << ">> " << get<0>(choice) << " [" << get_var(get<1>(choice), get<2>(choice)) << "]: ";
		getline(cin, line);

		if (!line.empty())
			set_var(get<1>(choice), get<2>(choice), line);
	}

	return module::type::menu;
}

std::string config_module::get_var(const var_type& type, void* ptr)
{
	switch (type)
	{
		case var_type::string_t:
			return *(static_cast<string*>(ptr));

		case var_type::char_t:
			return string(1, *(static_cast<char*>(ptr)));

		case var_type::int_t:
		case var_type::bool_t:
			return to_string(*(static_cast<int*>(ptr)));
	}

	return "";
}

void config_module::set_var(const var_type& type, void* ptr, const std::string &val)
{
	switch (type)
	{
		case var_type::string_t:
			*(static_cast<string*>(ptr)) = val;
			break;

		case var_type::char_t:
			*(static_cast<char*>(ptr)) = val[0];
			break;

		case var_type::int_t:
		case var_type::bool_t:
			*(static_cast<int*>(ptr)) = stoi(val);
			break;
	}
}

void config_module::print_choices()
{
	for (unsigned int i = 0; i < choices.size(); i++)
	{
		auto &choice = choices[i];
		cout << (i + 1) << ". " << get<0>(choice) << " [" << get_var(get<1>(choice), get<2>(choice)) << "]" << endl;
	}
}
