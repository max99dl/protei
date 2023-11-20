#pragma once

#include<boost/beast/core.hpp>
#include<boost/beast/websocket.hpp>
#include<boost/date_time/posix_time/posix_time.hpp>
#include<iostream>
#include<vector>
#include<memory>
#include<chrono>
#include<utility>//for std::pair<>
#include<algorithm>
#include<string>
#include<thread>
#include<unistd.h>//for sleep()
#include<fstream>//for work with files
#include<cstdlib>//for std::rand()


#define SEPARATOR "##################"
#define OUTPUT_FILE "clients_information.txt"
#define BUSY false
#define FREE true


using tcp = boost::asio::ip::tcp;
namespace beast = boost::beast;
namespace Time = boost::posix_time;

std::string get_current_time();//+
size_t speech_imitation(std::pair<size_t, size_t> min_max_time_range);//+
void inform_for_overload();//+




///////////////////////////////////////////////////////////////////////////////CLIENT
class Client {
public:
	Client(tcp::socket connection_socket_);
	~Client();
	std::string get_id() const;//+
	void set_number(std::string number);//+
	std::string get_number() const;//+
	void set_operator_time();//+
	void set_talk_duration(size_t talk_duration);
	//void set_socket_connection(tcp::socket connection_socket_);//+
	tcp::socket get_socket_connection();//+
	inline static size_t number_id = 0;//+
private:
	size_t talk_duration_s = 0;
	tcp::socket connection_socket;//+
	std::string unique_id;//+
	std::string phone_number;//+
	//information data-time
	std::string client_start_time;//+
	std::string operator_start_time;//+
};






//////////////////////////////////////////////////////////////////OPERATORS
class Operator {
public:
	Operator(size_t count_of_operators,
					 size_t max_waiting_queue_size,
					 std::pair<size_t, size_t> min_max_time);//+


  void add_client(const std::string client_ID);
  void remove_client_from_waiting_queue();
	size_t clients_is_waiting_count() const;//+
	bool check_waiting_count() const;
	//then create a client we should add him in waiting queue
	void add_waiting_client(std::string client_ID);//+
	//count of available operators
	size_t count_of_free_operators() const;//+
	std::pair<size_t, size_t> get_min_max_time_range() const;//+
	void find_and_remove_client(const std::string client_ID);//+
	size_t find_client_position(const std::string client_ID);
	void process_client(std::shared_ptr<Client> client);
private:
	std::vector<std::string> operators;//+
	//clients in waiting queue
	std::vector<std::string> clients_is_waiting;//+
	const size_t operators_size;//+
	const size_t max_waiting_size;//+
	const std::pair<size_t, size_t> min_max_time_range;
};

////////////////////////////////////////////////////////////////////////SERVER
class Server {
public:
	Server(const std::string& address_, int port_,
				 int context_hint,
				 size_t count_of_operators,
				 size_t max_waiting_queue_size,
				 std::pair<size_t, size_t> min_max_time_range);//+
	void start();//+
private:
	const boost::asio::ip::address address;//+
	const unsigned short port;//+
	const tcp::endpoint endpoint;//+
	boost::asio::io_context io_context;//+
	tcp::acceptor acceptor;//+
	Operator operators;//+
};



////////////////////////////////////////////////////////////////////FILE

class File {
public:
	File(const std::string file_name_);//+
	std::ofstream& get_file_descriptor();
	~File();
	void print_information(const std::string& message);//+
private:
	const std::string file_name;
	std::ofstream file_descriptor;
};



void set_client_stream(std::shared_ptr<Client> client, Operator& operators);//+

void set_client_waiting_stream(std::shared_ptr<Client> client, Operator& operators);

void send_message_to_client(beast::websocket::stream<tcp::socket>& ws,
														const std::string& message);

std::string receive_message_from_client(beast::websocket::stream<tcp::socket>& ws);
