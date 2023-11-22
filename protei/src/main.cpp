#include <web_server.hpp>
#include <thread>


#define PORT 8080
#define ADDRESS "127.0.0.1"


int main() {
	//add logger to log information
	MyLogger::MyLogger my_logger;
	size_t concurrency = std::thread::hardware_concurrency();
	if(concurrency == 0) {
		//log for warning--------------------------------------------------??
		concurrency = 2;
	}
	
	//next we need to parse json file config----------------------------??
	const auto [client_waiting_min_max_time_range,
							client_talking_min_max_time_range,
							waiting_queue_size, count_of_operators]
							= parse_config_file(CONFIG_FILE);							
	size_t io_context_hint = 1;
	
	my_logger.log_information(MyLogger::Status::INFO, "create and set up web server");

	Server server(ADDRESS, PORT, io_context_hint,
								count_of_operators, waiting_queue_size,
								client_talking_min_max_time_range,
								client_waiting_min_max_time_range);
								
	//log for start working webserver---------------------------------??
	server.start();
	return 0;
}
