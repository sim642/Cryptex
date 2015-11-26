#ifndef SRF_DONGLE_H
#define SRF_DONGLE_H

#include "serial_device.hpp"
#include <thread>
#include <mutex>
#include <atomic>
#include <string>
#include <vector>
#include <tuple>

class srf_dongle
{
	public:
		srf_dongle(boost::asio::io_service &io, const std::string &dev);
		virtual ~srf_dongle();

		void send(std::string raw);
		void send(char start, std::string id, std::string cmd);
		void send(char field, char target, std::string cmd);

		std::string recv_raw();
		std::tuple<char, std::string, std::string> recv();
		std::tuple<char, char, std::string> recv_parsed();

	private:
		void receiver();

		boost::asio::serial_port port;
		serial_stream stream;
		std::mutex stream_mut;

		std::thread thr;
		std::atomic_bool thr_running;

		std::vector<std::string> recvd;
		std::mutex recvd_mut;
};

#endif // SRF_DONGLE_H
