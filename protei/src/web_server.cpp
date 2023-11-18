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
		Client client;
		//connecting client
		//client.set_connection_socket(std::move(socket));
		//add client to waiting queue
		operators.add_waiting_client(std::move(client));
		operators.update_queues(socket);
		//notify which in waiting queue-----------------------------------------------------------??
		//our client is in the client_is_waiting.top()
		//if(waiting_queue_is_not_empty) then 
		//operators.information_for_waiting_clients();
	
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

void Operator::find_and_remove_client(std::string client_ID) {//+
	auto client_position =
			std::find(operators.begin(), operators.end(), client_ID);
	if(client_position != operators.end()) {
		operators.erase(client_position);
		} else {
			//error log that some problems
		}
}

size_t Operator::count_of_free_operators() const {//+
	return operators_size - operators.size();
}



size_t Operator::clients_is_waiting_count() const {//+
	return clients_is_waiting.size();
}

//need to pass a client by std::move()
void Operator::add_waiting_client(Client client) {//+
	clients_is_waiting.push_back(std::move(client));
}



bool Operator::check_waiting_count() const {//+
	return clients_is_waiting.size() < max_waiting_size;
}


void Operator::update_queues(tcp::socket& socket) {//+
	size_t check = std::min(count_of_free_operators(), clients_is_waiting_count());
	for(size_t i = 0; i < check; ++i) {
		//add CLient ID to operators queue
		operators.push_back(clients_is_waiting[0].get_id());
		//connect client with operator and sleep
		set_client_stream(std::move(clients_is_waiting[0]), *this, socket);
		//delete client from waiting queue
		clients_is_waiting.erase(clients_is_waiting.begin());
	}
}






/////////////////////////////////////////////////////////////////////////////////////////CLIENT
Client::Client()
{
	client_start_time = get_current_time();
	unique_id = "ID##"+std::to_string(Client::number_id++);
}
/*
Client::Client(Client&& other) {
	this->talk_duration_s = other.talk_duration_s;
	this->connection_socket = std::move(other.connection_socket);
	this->unique_id = other.unique_id;
	this->phone_number = other.phone_number;
	this->client_start_time = other.client_start_time;
	this->operator_start_time = other.operator_start_time;

	other.talk_duration_s = 0;
	other.unique_id.clear();
	other.phone_number.clear();
	other.client_start_time.clear();
	other.operator_start_time.clear();
}*/




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
	std::cout << "i am in set_operator_time" << std::endl;
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
void Clients::set_connection_socket(tcp::socket connection_socket_) {//+
	connection_socket = std::move(connection_socket_);
}

tcp::socket Clients::get_connection_socket() {//+
	return std::move(connection_socket);
}
*/


/////////////////////////////////////////////////////////////////FILE
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



void set_client_stream(Client cl, Operator& operators, tcp::socket& socket) {
	std::thread{ [client = std::move(cl), &operators, &socket] () mutable {
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
				//buffer for incoming message
				beast::flat_buffer buffer;
				//read ans set Client number
				ws.read(buffer);
				client.set_number(std::move(beast::buffers_to_string(buffer.cdata())));
				std::cout << client.get_number() << std::endl;
				//clear the buffer
				//buffer.clear();
				//write data
				const std::string message_to_client = "Your id: " + client.get_id();
				//ws.write(message_to_client);
				//set time operator response
				client.set_operator_time();
				//make client sleep for n seconds
				client.set_talk_duration(
					speech_imitation(operators.get_min_max_time_range())
				);
				//after sleep we need to remove client from operators queue
				operators.find_and_remove_client(client.get_id());
				//update waiting_clients_and_operators_queue
				operators.update_queues(socket);
				//operators.information_for_waiting_clients();
					
			} catch(const beast::system_error& se) {
				if(se.code() != beast::websocket::error::closed) {
					//log for error-------------------------------------------------------??
					std::cout << se.code().message() << std::endl;
				}
			}

	}}.detach();
}



