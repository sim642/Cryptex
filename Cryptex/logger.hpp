#ifndef LOGGER_H
#define LOGGER_H

#include <ostream>
#include <initializer_list>
#include <vector>
#include <chrono>

class logger
{
	public:
		logger();
		logger(std::initializer_list<std::ostream*> nostreams);
		virtual ~logger();

		template<typename T>
		logger& operator<< (const T &val)
		{
			for (auto &ptr : ostreams)
				(*ptr) << val;
			return *this;
		}

		logger& operator<< (std::ostream& (&func)(std::ostream&));

		template<typename T>
		void print(T&& val)
		{
			*this << val;
		}

		template<typename T, typename ...Ts>
		void print(T&& val, Ts&&... vals)
		{
			*this << val << " ";
			print(vals...);
		}

		template<typename ...Ts>
		void operator() (const std::string &category, Ts&&... vals)
		{
			*this << timestamp() << " [" << category << "] ";
			print(vals...);
			*this << std::endl;
		}

	private:
		std::string timestamp(bool fromstart = true);

		std::vector<std::ostream*> ostreams;
		std::chrono::system_clock::time_point starttime;
};

extern logger LOG;

#endif // LOGGER_H
