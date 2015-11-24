#ifndef LOGGER_H
#define LOGGER_H

#include <ostream>
#include <initializer_list>
#include <vector>

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
		void operator() (Ts&&... vals)
		{
			*this << "[" << timestamp() << "] ";
			print(vals...);
			*this << std::endl;
		}

	private:
		std::string timestamp();

		std::vector<std::ostream*> ostreams;
};

extern logger log;

#endif // LOGGER_H
