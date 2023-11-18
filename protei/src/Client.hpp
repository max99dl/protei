#pragma once

#include<unistd.h>//for sleep()
#include<thread>
#include<fstream>
#include<string>
#include<cstdlib>//for std::rand()

#define BUSY 0
#define FREE 1

using Time = boost::posix_time;


void work(bool& flag);

class Client {
public:
	Client();
	~Client();
	void start_work_with_operator();
	bool check() const;
	std::string get_id() const;
	void set_number(const std::string& number);
	void set_operator_time(std::string current_time);
	inline static int number_id = 0;
private:
	bool flag = false;
	std::string unique_id;
	std::string phone_number;
	std::string client_start_time;
	std::string client_finish_time;
	std::string operator_start_time;
};


class File {
public:
	File(const std::string file_name_);
	std::fstream& get_file_descriptor() const;
	~File();
private:
	const std::string file_name;
	std::fstream file_descriptor;
}
