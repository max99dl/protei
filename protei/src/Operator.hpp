#pragma once

#include "Client.hpp"
#include<vector>
#include<string>
#include<algorithm>

class Operator {
public:
	Operator(size_t count_of_operators,
					 size_t max_waiting_queue_size);
	void update_operators() const;
	//try to add client
	bool add_client();
	// is there are available operator
	bool check_for_free() const;
	//count of currently busy operators
	size_t get_current_size() const;
	size_t clients_is_waiting_count() const;
	//then create a client we should add him in waiting queue
	void add_waiting_client(Client client);
	//count of available operators
	size_t count_of_free_operators() const;
	bool check_waiting_count() const;
	void update_queues();
private:
	//current_working_clients, we make it mutable to be able change in const method update_operators()
	mutable std::vector<std::string> operators;
	//clients in waiting queue
	std::vector<Client> clients_is_waiting;
	const size_t operators_size;
	const size_t max_waiting_size;
};
