#include "web_server.hpp"



///////////////////////////////////////////////////////////////////////////SERVER
Server::Server(const std::string& address_, int port_,
				 			 int context_hint,
				 			 size_t count_of_operators,
				 			 size_t max_waiting_queue_size,
				 			 std::pair<size_t, size_t> min_max_time_range)//+
		: address(boost::asio::ip::make_address(address_)),
			port(static_cast<unsigned short>(port_)),
			endpoint(tcp::endpoint(address, port)),
			io_context(context_hint),
			acceptor(io_context, endpoint),
			operators(count_of_operators, max_waiting_queue_size,
								min_max_time_range)
{
	//log for server has been created------------------------------------------??
}


void Server::start() {//+
	//log for starting worling server------------------------------------------------??
	while(1) {
		//check for correct client connection
		boost::system::error_code error;
		tcp::socket socket {io_context};
		acceptor.accept(socket, error);
		if(error) {
			//log for error----------------------------------------------------------------??
			std::cerr << "Error: bad connection" << error << std::endl;
			
		} else if(!operators.check_waiting_count()) {//check for waiting queue size
			//log for overload--------------------------------------------------------------??
			File file(OUTPUT_FILE);
			file.print_information("overload");
			continue;
		}
		
		
		auto client = std::make_shared<Client>(std::move(socket));
		//client->set_socket_connection(std::move(socket));
		
		operators.process_client(client);
	}
}



//////////////////////////////////////////////////////////////////OPERATORS////
Operator::Operator(size_t size, size_t waiting_size,
									 std::pair<size_t, size_t> min_max_time)
	: operators_size(size),
		max_waiting_size(waiting_size),//+
		min_max_time_range(min_max_time)
{
	operators.reserve(operators_size);
	clients_is_waiting.reserve(max_waiting_size);
}

std::pair<size_t, size_t> Operator::get_min_max_time_range() const {//+
	return min_max_time_range;
}

void Operator::find_and_remove_client(const std::string client_ID) {//+
	auto client_position =
			std::find(operators.begin(), operators.end(), client_ID);
	if(client_position != operators.end()) {
		operators.erase(client_position);
		} else {
			//error log that some problems
		}
}

size_t Operator::find_client_position(const std::string client_ID) {
	auto client_position =
		std::find(clients_is_waiting.begin(),
							clients_is_waiting.end(), client_ID);
	if(client_position != clients_is_waiting.end()) {
		return client_position - clients_is_waiting.begin() + 1;
		} else {
			//error log that some problems----------------------------------------------------??
			return 0;
		}
}

void Operator::add_client(const std::string client_ID) {
	operators.push_back(client_ID);
}

void Operator::remove_client_from_waiting_queue() {
	if(clients_is_waiting.empty()) {
		//log for error-----------------------------------------------------??
	} else {
		clients_is_waiting.erase(clients_is_waiting.begin());
	}
}

size_t Operator::count_of_free_operators() const {//+
	return operators_size - operators.size();
}



size_t Operator::clients_is_waiting_count() const {//+
	return clients_is_waiting.size();
}

bool Operator::check_waiting_count() const {
	return clients_is_waiting.size() < max_waiting_size;
}
	
//need to pass a client by std::move()
void Operator::add_waiting_client(std::string client_ID) {//+
	clients_is_waiting.push_back(client_ID);
}

void Operator::process_client(std::shared_ptr<Client> client) {
	if((operators.size() < operators_size) && clients_is_waiting.empty()) {
		//we can now connect client with free operator
		operators.push_back(client->get_id());
		set_client_stream(client, *this);
	} else {
		//we need to add client in waiting queue
		clients_is_waiting.push_back(client->get_id());
		set_client_waiting_stream(client, *this);
	}
}





/////////////////////////////////////////////////////////////////////////////////////////CLIENT
Client::Client(tcp::socket connection_socket_) : connection_socket(std::move(connection_socket_))
{
	client_start_time = get_current_time();
	unique_id = "ID##"+std::to_string(Client::number_id++);
}

Client::~Client() {
  //need to log client information
	std::string client_finish_time = get_current_time();
	File file(OUTPUT_FILE);
	std::ofstream& descriptor = file.get_file_descriptor();
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

void Client::set_operator_time() {//+
	operator_start_time = get_current_time();
}

std::string Client::get_number() const {//+
	return phone_number;
}
	
void Client::set_number(std::string number) {//+
	phone_number = number;
}

std::string Client::get_id() const {//+
	return unique_id;
}

void Client::set_talk_duration(size_t talk_duration) {
	talk_duration_s = talk_duration;
}
/*
void Client::set_socket_connection(tcp::socket connection_socket_) {//+
	connection_socket = std::move(connection_socket_);
}*/

tcp::socket Client::get_socket_connection() {//+
	return std::move(connection_socket);
}



////////////////////////////////////////////////////////////////////////////////////////FILE
File::File(const std::string file_name_)
	: file_name(file_name_),
	  file_descriptor(file_name, std::ios::app)//+
{
//log for writing in file-----------------------------------------------------------??
	if(!file_descriptor) {
		//log for error, could not open file--------------------------------------------??
	}
}

std::ofstream& File::get_file_descriptor() {
	return file_descriptor;
}

void File::print_information(const std::string& message) {//+
	file_descriptor << SEPARATOR << std::endl
									<< message << std::endl
									<< SEPARATOR << std::endl;
}

File::~File() {
	file_descriptor.close();
	//log for closing file-------------------------------------------------------------??
}



std::string get_current_time() {//+
	return to_simple_string(Time::microsec_clock::universal_time());
}

size_t speech_imitation(std::pair<size_t, size_t> min_max_time_range) {//+
	//create speech_duration
	auto [time_min, time_max] = min_max_time_range;
	size_t busy_time = 
		static_cast<size_t>(std::rand() % (time_max - time_min)) + time_min;
	std::cout << "i am in speech_imitation " << busy_time << "s" << std::endl;
	size_t actual_time_duration = 0;	
	for(size_t i = 0; i < busy_time; ++i) {
		sleep(1);
		++actual_time_duration;
	}
	if(busy_time != actual_time_duration) {
		//log for error---------------------------------------------------------------??
	}
	return actual_time_duration;
}



//the last argement needs to clarify client is in waiting queue or not
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
				//write data
				const std::string message_to_client = "Your id: " + client->get_id();
				send_message_to_client(ws, message_to_client);
				//set time operator response
				client->set_operator_time();
				//make client sleep for n seconds
				client->set_talk_duration(
					speech_imitation(operators.get_min_max_time_range())
				);
				//after sleep we need to remove client from operators queue
				operators.find_and_remove_client(client->get_id());

			} catch(const beast::system_error& se) {
				if(se.code() != beast::websocket::error::closed) {
					//log for error-------------------------------------------------------??
					std::cout << se.code().message() << std::endl;
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

				//send message to client which in wiating_queue
				size_t order_in_waiting_queue = operators.clients_is_waiting_count();
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
						//there are free operator and client in waiting queue is first
						operators.add_client(client->get_id());
						operators.remove_client_from_waiting_queue();
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
					speech_imitation(operators.get_min_max_time_range())
				);
				//after sleep we need to remove client from operators queue
				operators.find_and_remove_client(client->get_id());
					
			} catch(const beast::system_error& se) {
				if(se.code() != beast::websocket::error::closed) {
					//log for error-------------------------------------------------------??
					std::cout << se.code().message() << std::endl;
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
