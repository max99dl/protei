#include "Operator.hpp"
#include <algorithm>
#include <vector>
#include "Client.hpp"

Operator::Operator(size_t size, size_t waiting_size)
	: operators_size(size),
		max_waiting_size(waiting_size)
{
	operators.reserve(operators_size);
	clients_is_waiting.reserve(max_waiting_size);
}


size_t Operator::get_current_size() const {
	update_operators();
	return operators.size();
}

bool Operator::check_for_free() const {
	return get_current_size() < operators_size;
}

bool Operator::add_client() {
	if(check_for_free()) {
		
		//remove client fom waiting queue to operators queue
		operators.push_back(std::move(clients_is_waiting[0]));
		operators[operators.size()-1].start_work_with_operator();
		//first client in waiting queue start work with operator
		//clients_is_waiting[0].start_work_with_operator();
		//update wiating queue size
		clients_is_waiting.erase(clients_is_waiting.begin());
		return 1;
	}
	
	return 0;
}

void Operator::update_operators() const {
	auto new_end_iter = std::remove_if(operators.begin(), operators.end(),
																		 [](const Client& client) {
		return client.check();
	});
	operators.erase(new_end_iter, operators.end());
}

size_t Operator::clients_is_waiting_count() const{
	return clients_is_waiting.size();
}

//need to pass a client by std::move()
void Operator::add_waiting_client(Client client) {
	clients_is_waiting.push_back(std::move(client));

	//then we can try to add client in working with an operator
	add_client();
}


size_t Operator::count_of_free_operators() const {
	return operators_size - operators.size();
}

bool Operator::check_waiting_count() const {
	return clients_is_waiting.size() < max_waiting_size;
}


void Operator::update_queues() {
	size_t ckeck = std::min(count_of_free_operators(), clients_is_waiting_count());
	for(size_t )
}
