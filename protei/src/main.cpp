#include "web_server.hpp"
#include <thread>


#define PORT 8080
#define ADDRESS "127.0.0.1"

int main() {
	size_t concurrency = std::thread::hardware_concurrency();
	if(concurrency == 0) {
		//log for warning--------------------------------------------------??
		concurrency = 2;
	}
	std::cout << concurrency << std::endl;
	
	//next we need to parse json file config----------------------------??
	
	const size_t max_operators_count = 3;
	const size_t max_waiting_count = 3;
	size_t io_context_hint = 1;
	size_t min_time_speech = 5;
	size_t max_time_speech = 10;
	auto min_max_time_range = std::make_pair(min_time_speech, max_time_speech);
	
//log for create and set up server-----------------------------------??
	Server server(ADDRESS, PORT, io_context_hint,
								max_operators_count, max_waiting_count,
								min_max_time_range);
								
	//log for start working webserver---------------------------------??
	server.start();
	return 0;
}
