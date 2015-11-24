#include "logger.hpp"
#include <ctime>
#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>

using namespace std;

ofstream logfile("log.txt", ios_base::app);
logger log = {&cout, &logfile};

logger::logger() : starttime(std::chrono::system_clock::now())
{

}

logger::logger(std::initializer_list<std::ostream*> nostreams) : ostreams(nostreams), starttime(std::chrono::system_clock::now())
{

}

logger::~logger()
{

}

logger& logger::operator<< (std::ostream& (&func)(std::ostream&))
{
	for (auto &ptr : ostreams)
		(*ptr) << func;
	return *this;
}

std::string logger::timestamp(bool fromstart)
{
	auto now = chrono::system_clock::now();
	auto now_c = chrono::system_clock::to_time_t(now);

	char str[128];
	strftime(str, 128, "%F %T", localtime(&now_c));

	if (fromstart)
	{
		char str2[64];

		time_t dur = chrono::duration_cast<chrono::seconds>(now - starttime).count();
		auto dur_c = gmtime(&dur);

		strftime(str2, 64, " %M:%S", dur_c);
		strcat(str, str2);
	}
	return string(str);
}
