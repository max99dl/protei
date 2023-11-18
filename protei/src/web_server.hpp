#pragma once

#include<boost/beast/core.hpp>
#include<boost/beast/websocket.hpp>
#include<boost/date_time/posix_time/posix_time.hpp>
#include<iostream>
#include<vector>
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


using tcp = boost::asio::ip::tcp;
namespace beast = boost::beast;
namespace Time = boost::posix_time;

std::string get_current_time();//+
size_t speech_imitation(std::pair<size_t, size_t> min_max_time_range);//+
void inform_for_overload();//+




///////////////////////////////////////////////////////////////////////////////CLIENT
class Client {
public:
	Client();
	~Client();
	std::string get_id() const;//+
	void set_number(std::string number);//+
	std::string get_number() const;//+
	void set_operator_time();//+
	void set_talk_duration(size_t talk_duration);
	//void set_connection_socket(tcp::socket connection_socket_);//+
	//tcp::socket get_connection_socket();//+
	inline static size_t number_id = 0;//+
private:
	size_t talk_duration_s = 0;
	//tcp::socket connection_socket;//+
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


	size_t clients_is_waiting_count() const;//+
	//then create a client we should add him in waiting queue
	void add_waiting_client(Client client);//+
	//count of available operators
	size_t count_of_free_operators() const;//+
	bool check_waiting_count() const;//+
	void update_queues(tcp::socket& socket);//+
	std::pair<size_t, size_t> get_min_max_time_range() const;
	void find_and_remove_client(std::string client_ID);
private:
	//current_working_clients, we make it mutable to be able change in const method update_operators()
	std::vector<std::string> operators;//+
	//clients in waiting queue
	std::vector<Client> clients_is_waiting;//+
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



void set_client_stream(Client client, Operator& operators, tcp::socket& socket);//+
//void work(tcp::socket connection_socket, )

