#include "Client.hpp"
#include<thread>
#include<string>
#include<iostream>
#include<unistd.h>//for sleep()
#include<cstdlib>//for std::rand
	
using namespace std::chrono;

void work(bool& flag) {
	//min time for work with client is 15s, max is 44s
	size_t busy_time = static_cast<size_t>(std::rand() % 30) + 15u;
	std::cout << &flag << std::endl;
	std::cout << busy_time << 's' <<  std::endl; //add for debug
	for(size_t i = 0; i < busy_time; ++i) {
		sleep(1);
	}
	flag = true;
	std::cout << "time out from client" << std::endl; //add for debug
}

void Client::start_work_with_operator() {
	std::cout << &flag << std::endl;
	std::thread t {work, std::ref(flag)};
	t.detach();
}

bool Client::check() const {
	return flag;
}





File::File(const std::string file_name_)
	: file_name(file_name_),
	  file_descriptor(file_name, std::ios::app) 
{}

std::fstream& File::get_file_descriptor() const {
	return file_descriptor;
}

File::~File() {
	file_descriptor.close();
}
