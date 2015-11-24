#include "logger.hpp"
#include <ctime>
#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

using namespace std;

ofstream logfile("log.txt", ios_base::app);
logger log = {&cout, &logfile};

logger::logger()
{

}

logger::logger(std::initializer_list<std::ostream*> nostreams) : ostreams(nostreams)
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

std::string logger::timestamp()
{
	auto now = chrono::system_clock::now();
	auto now_c = chrono::system_clock::to_time_t(now);

	string str(64, '\0');
	strftime(&*str.begin(), str.size(), "%F %T", localtime(&now_c));
	return str;
}
