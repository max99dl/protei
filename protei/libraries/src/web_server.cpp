#include "web_server.hpp"


//we make logger variable global,
//because  we don't want to send it as a parameter
//to all our functions and methods
MyLogger::MyLogger my_logger;


///////////////////////////////////////////////////////////////////////////SERVER
Server::Server(const std::string& address_, int port_,
				 			 int context_hint,
				 			 size_t count_of_operators,
				 			 size_t max_waiting_queue_size,
				 			 std::pair<size_t, size_t> client_talking_min_max_time_range,
				 			 std::pair<size_t, size_t> client_waiting_min_max_time_range)
		: address(boost::asio::ip::make_address(address_)),
			port(static_cast<unsigned short>(port_)),
			endpoint(tcp::endpoint(address, port)),
			io_context(context_hint),
			acceptor(io_context, endpoint),
			operators(count_of_operators, max_waiting_queue_size,
								client_talking_min_max_time_range,
								client_waiting_min_max_time_range)
{
	std::string message = "Srever has been created";
	my_logger.log_information(MyLogger::Status::INFO, message);
}


void Server::start() {
	
	std::string message = "Server started working";
	my_logger.log_information(MyLogger::Status::INFO, message);

	while(1) {
		//check for correct client connection
		boost::system::error_code error;
		tcp::socket socket {io_context};
		acceptor.accept(socket, error);
		if(error) {	
			std::string message = "Bad connection with client socket";
			my_logger.log_information(MyLogger::Status::ERROR, message);
		} else if(!operators.check_waiting_count()) {//check for waiting queue size
			my_logger.log_information(MyLogger::Status::WARN, "overload");
			File file(OUTPUT_FILE);
			file.print_information("overload");
			continue;
		}
		//create cleint
		auto client = std::make_shared<Client>(std::move(socket));
		std::string message = "request from client " + client->get_id();
		my_logger.log_information(MyLogger::Status::INFO, message);
		
		//try work with client
		try {
			operators.process_client(client);
		} catch(std::runtime_error& re) {
			my_logger.log_information(MyLogger::Status::ERROR, re.what());
		}
	}
}

/////////////////////////////////////////////////////////////////////////OPERATORS
Operator::Operator(size_t size, size_t waiting_size,
									 std::pair<size_t, size_t> client_talking_min_max_time_range_,
									 std::pair<size_t, size_t> client_waiting_min_max_time_range_)
	: operators_size(size),
		max_waiting_size(waiting_size),
		client_talking_min_max_time_range(client_talking_min_max_time_range_),
		client_waiting_min_max_time_range(client_waiting_min_max_time_range_)
{
	operators.reserve(operators_size);
}

void Operator::find_and_remove_client(const std::string client_ID) {
	std::lock_guard g(mutex);
	auto client_position =
			std::find(operators.begin(), operators.end(), client_ID);
	if(client_position != operators.end()) {
		*client_position = std::move(operators.back());
		operators.pop_back();
		} else {
			const std::string message =
				"can't find client " + client_ID + " which must be in operator's queue";
			throw std::runtime_error(message);
		}
}

size_t Operator::find_client_position(const std::string client_id) {
	auto client_position =
		std::find(clients_is_waiting.begin(),
							clients_is_waiting.end(), client_id);
	if(client_position != clients_is_waiting.end()) {
		return client_position - clients_is_waiting.begin() + 1;
		} else {
			const std::string message = "coudn't find client " +
																	client_id +
																	" in waiting queue, which must be there";
			throw std::runtime_error(message);
			return 0;
		}
}

void Operator::add_client(const std::string& client_ID) {
	operators.push_back(client_ID);
}

size_t Operator::count_of_free_operators() const {
	return operators_size - operators.size();
}

void Operator::remove_client_from_waiting_queue(const std::string& client_id) {
	std::lock_guard g(mutex);
	auto client_position =
		std::find(clients_is_waiting.begin(), clients_is_waiting.end(), client_id);
		if(client_position != clients_is_waiting.end()) {
			clients_is_waiting.erase(client_position);
		} else {
			const std::string message = "coudn't find client " +
																	client_id +
																	" in waiting queue, which must be there";
			throw std::runtime_error(message);
		}
}

void Operator::remove_client_phone_number(const std::string& client_number) {
	std::lock_guard g(mutex);
	auto number_position = std::find(clients_phone_numbers.begin(), 
																	 clients_phone_numbers.end(),
																	 client_number);
	if(number_position != clients_phone_numbers.end()) {
		*number_position = std::move(clients_phone_numbers.back());
		clients_phone_numbers.pop_back();
	} else {
		const std::string message = "coudn't find client number " +
																client_number +
																" in storage, which must be there";
		throw std::runtime_error(message);
	}	
}

void Operator::set_client_phone_number(const std::string& client_number) {
	clients_phone_numbers.push_back(client_number);
}


size_t Operator::clients_is_waiting_count() const {
	return clients_is_waiting.size();
}

bool Operator::check_waiting_count() const {
	return clients_is_waiting.size() < max_waiting_size;
}
	
//need to pass a client by std::move()
void Operator::add_waiting_client(std::string client_ID) {//+
	clients_is_waiting.push_back(client_ID);
}

bool Operator::check_for_unique_phone_number(const std::string& phone_number) const {
	bool result = std::count(clients_phone_numbers.begin(),
													 clients_phone_numbers.end(),
													 phone_number);
	return !result;
}

std::pair<size_t, size_t> Operator::get_client_waiting_time_range() const {
	return client_waiting_min_max_time_range;
}

std::pair<size_t, size_t> Operator::get_client_talking_time_range() const {
	return client_talking_min_max_time_range;
}

void Operator::process_client(std::shared_ptr<Client> client) {
	if((operators.size() < operators_size) && clients_is_waiting.empty()) {
		set_client_stream(client, *this);
	} else {
		set_client_waiting_stream(client, *this);
	}
}


/////////////////////////////////////////////////////////////////////////////////////////CLIENT
Client::Client(tcp::socket connection_socket_)
	: connection_socket(std::move(connection_socket_))
{
	client_start_time = get_current_time();
	unique_id = "ID##"+std::to_string(Client::number_id++);
}

Client::~Client() {
	if(flag) {
		std::lock_guard g(mutex);
  	//need to log client information
		std::string client_finish_time = get_current_time();
		File file(OUTPUT_FILE);
		std::fstream& descriptor = file.get_file_descriptor();
		//write into the file
		descriptor << SEPARATOR << std::endl
				<< "Start: " << client_start_time << std::endl
				<< "Call ID: " << unique_id << std::endl
				<< "Client number: " << phone_number << std::endl
				<< "Finish: " << client_finish_time << std::endl
				<< "Operator response: " << operator_start_time << std::endl
				<< "Conversation duration: " << talk_duration_s << std::endl
				<< SEPARATOR << std::endl;		
	}
}

void Client::set_operator_time() {
	operator_start_time = get_current_time();
}

std::string Client::get_number() const {
	return phone_number;
}
	
void Client::set_number(std::string number) {
	phone_number = number;
}

std::string Client::get_id() const {
	return unique_id;
}

void Client::set_talk_duration(size_t talk_duration) {
	talk_duration_s = talk_duration;
}

tcp::socket Client::get_socket_connection() {
	return std::move(connection_socket);
}

void Client::set_flag_to_false() {
	flag = false;
}

////////////////////////////////////////////////////////////////////////////////////////FILE
File::File(const std::string file_name_)
	: file_name(file_name_),
	  file_descriptor(file_name, std::ios::app)
{
	std::string message = "open file " + file_name;
	my_logger.log_information(MyLogger::Status::DEBUG, message);
	
	if(!file_descriptor) {
		message = "can't open file " +
							file_name +
							" to write information";
		throw std::runtime_error(message);
	}
}

std::fstream& File::get_file_descriptor() {
	return file_descriptor;
}

void File::print_information(const std::string& message) {
	file_descriptor << SEPARATOR << std::endl
									<< message << std::endl
									<< SEPARATOR << std::endl;
}

File::~File() {
	file_descriptor.close();
	const std::string message = "close file " + file_name;
		my_logger.log_information(MyLogger::Status::DEBUG, message);
}

///////////////////////////////////////////////////////////////////////////////////////////////FUNCTIONS
std::string get_current_time() {
	return to_simple_string(Time::microsec_clock::universal_time());
}

size_t get_random_time_in_range(const std::pair<size_t, size_t>& min_max_range) {
	auto [time_min, time_max] = min_max_range;
	size_t busy_time = 
		static_cast<size_t>(std::rand() % (time_max - time_min)) + time_min;
	return busy_time;
}

size_t speech_imitation(std::pair<size_t, size_t> min_max_time_range) {
	//create speech_duration
	size_t busy_time = get_random_time_in_range(min_max_time_range);
	size_t actual_time_duration = 0;	
	for(size_t i = 0; i < busy_time; ++i) {
		sleep(1);
		++actual_time_duration;
	}
	if(busy_time != actual_time_duration) {
		const std::string message = "theoretical talking time: " +
																std::to_string(busy_time) +
																"s not equal to real talk duration time: " +
																std::to_string(actual_time_duration) + "s";
		my_logger.log_information(MyLogger::Status::WARN, message);
	}
	return actual_time_duration;
}



void set_client_stream(std::shared_ptr<Client> client, Operator& operators) {
	std::thread{ [client, &operators, socket = std::move(client->get_socket_connection())] () mutable {
			beast::websocket::stream<tcp::socket> ws {
					std::move(socket)};
	
			//set 
			ws.set_option(beast::websocket::stream_base::decorator (
				[](beast::websocket::response_type& res) {
				res.set(beast::http::field::server, "operators");
			}));
				
			//connect with client
			ws.accept();

			try {
				client->set_number(std::move(receive_message_from_client(ws)));
				std::cout << client->get_number() << std::endl;
				
				//need to check is this phone number already in queue
				if(!operators.check_for_unique_phone_number(client->get_number())) {
					client->set_flag_to_false();
					send_message_to_client(ws, "already in queue");
					const std::string message =
							"request from client with phone number which already is processing";
					my_logger.log_information(MyLogger::Status::WARN, message);
					//number already exist, so should close the connection
					//can leave the thread
					return;
				}
				
				//next we can connect client with free operator
				operators.set_client_phone_number(client->get_number());
				const std::string message_to_log = "connecting client " +
						client->get_id() + " with free operator";
				my_logger.log_information(MyLogger::Status::INFO, message_to_log);
				//we can now connect client with free operator
				operators.add_client(client->get_id());
				
				
				//write data
				const std::string message_to_client = "Your id: " + client->get_id();
				send_message_to_client(ws, message_to_client);
				//set time operator response
				client->set_operator_time();
				//make client sleep for n seconds
				client->set_talk_duration(
					speech_imitation(operators.get_client_talking_time_range())
				);
				//after sleep we need to remove client from operators queue
				operators.find_and_remove_client(client->get_id());
				operators.remove_client_phone_number(client->get_number());
				send_message_to_client(ws, "good bye...");

			} catch(const beast::system_error& se) {
				if(se.code() != beast::websocket::error::closed) {
					const std::string message =
						"problem with send || receive information with client "
						+ client->get_id() + " " +	se.code().message();
					my_logger.log_information(MyLogger::Status::ERROR, message);
				}
			}

	}}.detach();
}



void set_client_waiting_stream(std::shared_ptr<Client> client, Operator& operators) {
	std::thread{ [client, &operators, socket = std::move(client->get_socket_connection())] () mutable {
			beast::websocket::stream<tcp::socket> ws {
					std::move(socket)};
	
			//set 
			ws.set_option(beast::websocket::stream_base::decorator (
				[](beast::websocket::response_type& res) {
				res.set(beast::http::field::server, "operators");
			}));
				
			//connect with client
			ws.accept();

			try {
				//write client phone number
				client->set_number(std::move(receive_message_from_client(ws)));
				std::cout << client->get_number() << std::endl;
				
				//need to check is this phone number already in queue
				if(!operators.check_for_unique_phone_number(client->get_number())) {
					client->set_flag_to_false();
					send_message_to_client(ws, "already in queue");
					const std::string message =
							"request from client with phone number which already is processing";
					my_logger.log_information(MyLogger::Status::WARN, message);
					//can leave the thread
					return;
				}
				
				//now we can prepare client to add in waiting queue
				operators.set_client_phone_number(client->get_number());
				const std::string message_to_log =
				"add client " + client->get_id() + " to waiting queue";
				my_logger.log_information(MyLogger::Status::INFO, message_to_log);
				//we need to add client in waiting queue
				operators.add_waiting_client(client->get_id());
				

				//send message to client which in waiting_queue
				size_t order_in_waiting_queue = operators.clients_is_waiting_count();
				size_t max_waiting_time =
						get_random_time_in_range(operators.get_client_waiting_time_range());
				size_t actual_time_in_waiting_queue = 0;
				std::string message_to_client =
						"You are " +
						std::to_string(order_in_waiting_queue) +
						" in waiting queue";
				send_message_to_client(ws, message_to_client);
				
				
				//being in waiting queue while there are no free operators
				while(1) {
					//client position in waiting queue
					size_t client_position = operators.find_client_position(client->get_id());
					if((operators.count_of_free_operators()) && client_position == 1) {
						const std::string message =
								"remove client " + client->get_id() + " from waiting queue "
								+ "and connect with free operator";
						my_logger.log_information(MyLogger::Status::INFO, message);
						//there are free operator and client in waiting queue is first
						operators.add_client(client->get_id());
						operators.remove_client_from_waiting_queue(client->get_id());
						break;
					}
					if(client_position != order_in_waiting_queue) {
						order_in_waiting_queue = client_position;
						message_to_client =
								"You are " +
								std::to_string(order_in_waiting_queue) +
								" in waiting queue";
						send_message_to_client(ws, message_to_client);
					}
					//if max client waiting time exceeded, then client close connection
					if(++actual_time_in_waiting_queue == max_waiting_time) {
						operators.remove_client_from_waiting_queue(client->get_id());
						operators.remove_client_phone_number(client->get_number());
						const std::string message =
								"client " + client->get_id() +
								" close connection because waiting time has been exceed";
						my_logger.log_information(MyLogger::Status::WARN, message);
						client->set_flag_to_false();
						send_message_to_client(ws, "good bye...");
						//can leave the thread
						return;
					}
					//if there are no free operators, keep stay in waiting queue
					sleep(1);
				}
				
				//now we ready to work with operator
				//send client ID
				message_to_client = "Your id: " + client->get_id();
				send_message_to_client(ws, message_to_client);
				//set time operator response
				client->set_operator_time();
				//make client sleep for n seconds
				client->set_talk_duration(
					speech_imitation(operators.get_client_talking_time_range())
				);
				//after sleep we need to remove client from operators queue
				operators.find_and_remove_client(client->get_id());
				operators.remove_client_phone_number(client->get_number());
				send_message_to_client(ws, "good bye...");
				
					
			} catch(const beast::system_error& se) {
				if(se.code() != beast::websocket::error::closed) {
					const std::string message =
						"problem with send || receive information with client "
						+ client->get_id() + " " +	se.code().message();
					my_logger.log_information(MyLogger::Status::ERROR, message);
				}
			}

	}}.detach();
}




void send_message_to_client(beast::websocket::stream<tcp::socket>& ws,
														const std::string& message) {
	//buffer for message
	beast::flat_buffer buffer;
	boost::beast::ostream(buffer) << message << std::endl;
	ws.write(buffer.data());
}


std::string receive_message_from_client(beast::websocket::stream<tcp::socket>& ws) {
	//buffer for incoming message
	beast::flat_buffer buffer;
	ws.read(buffer);
	return beast::buffers_to_string(buffer.cdata());
}


std::tuple<std::pair<size_t, size_t>,
					 std::pair<size_t, size_t>,
					 size_t, size_t>
 parse_config_file(const std::string& config_file) {
	//parse
  boost::property_tree::ptree rootHive;
  boost::property_tree::read_json(config_file, rootHive);
  
  auto waiting_min = rootHive.get<size_t>("Server.client_time_waiting_in_queue.min", 0);
  auto waiting_max = rootHive.get<size_t>("Server.client_time_waiting_in_queue.max", 0);
  auto talk_min = rootHive.get<size_t>("Server.client_time_work_with_operator.min", 0);
  auto talk_max = rootHive.get<size_t>("Server.client_time_work_with_operator.max", 0);
  auto waiting_queue = rootHive.get<size_t>("Server.waiting_queue_size", 0);
  auto operators_count = rootHive.get<size_t>("Server.count_of_operators", 0);
  //check for correct input
  if( (waiting_max < waiting_min) || (talk_max < talk_min)) {
  	my_logger.log_information(MyLogger::Status::CRITICAL, "invalid arguments in config file");
  	//can break the program
  	std::terminate();
  }
	//return parsed values
	return std::make_tuple(std::make_pair(waiting_min, waiting_max),
												 std::make_pair(talk_min, talk_max),
												 waiting_queue, operators_count);
}
