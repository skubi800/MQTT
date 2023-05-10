#pragma once
#define _WIN32_WINNT 0x0601

#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <string>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <deque>
#include <mutex>
#include <thread>
#include <optional>
#include <vector>
#include <algorithm>
#include <chrono>
#include <unordered_map>
#include "MQTTBroker.h"

using boost::asio::ip::tcp;
using namespace std;


int main() {
	boost::asio::io_context asio_context;

	MQTTbroker broker(asio_context);
	broker.startBroker();

	int numThreads = thread::hardware_concurrency() - 1;
	vector<thread> threads;

	for (int i = 0; i < numThreads; ++i) {
		threads.emplace_back([&asio_context]() {
			asio_context.run();
			});
	}

	asio_context.run();

	for (auto& thread : threads) {
		thread.join();
	}

	return 0;
}