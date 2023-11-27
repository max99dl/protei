#include "gtest/gtest.h"
#include "web_server.hpp"


TEST(OperatorFixture, add_client) {
	//Arrange
	const size_t count_of_operators = 3;
	const size_t max_waiting_queue_size = 3;
	std::pair<size_t, size_t> client_talking_min_max_time_range = std::make_pair(5u, 5u);
	std::pair<size_t, size_t> client_waiting_min_max_time_range = std::make_pair(5u, 5u);
	
	Operator operators(count_of_operators, max_waiting_queue_size,
										 client_talking_min_max_time_range,
										 client_waiting_min_max_time_range);

	const std::string client_id = "ID##10";
	const size_t expected_count = 2;
	
	//Act
	operators.add_client(client_id);
	
	//Assert
	ASSERT_EQ(expected_count, operators.count_of_free_operators());
}


TEST(OperatorFixture, add_waiting_client) {
	//Arrange
	const size_t count_of_operators = 3;
	const size_t max_waiting_queue_size = 3;
	std::pair<size_t, size_t> client_talking_min_max_time_range = std::make_pair(5u, 5u);
	std::pair<size_t, size_t> client_waiting_min_max_time_range = std::make_pair(5u, 5u);
	
	Operator operators(count_of_operators, max_waiting_queue_size,
										 client_talking_min_max_time_range,
										 client_waiting_min_max_time_range);
	
	const std::string client_id = "ID##10";
	const size_t expected_count = 1;
	
	//Act
	operators.add_waiting_client(client_id);
	
	//Assert
	ASSERT_EQ(expected_count, operators.clients_is_waiting_count());
}


TEST(OperatorFixture, remove_client_from_waiting_queue) {
	//Arrange
	const size_t count_of_operators = 3;
	const size_t max_waiting_queue_size = 3;
	std::pair<size_t, size_t> client_talking_min_max_time_range = std::make_pair(5u, 5u);
	std::pair<size_t, size_t> client_waiting_min_max_time_range = std::make_pair(5u, 5u);
	
	Operator operators(count_of_operators, max_waiting_queue_size,
										 client_talking_min_max_time_range,
										 client_waiting_min_max_time_range);
	
	const std::string client_id = "ID##10";
	const size_t expected_count = 0;
	
	//Act
	operators.add_waiting_client(client_id);
	operators.remove_client_from_waiting_queue(client_id);
	
	//Assert
	ASSERT_EQ(expected_count, operators.clients_is_waiting_count());
}


TEST(OperatorFixture, find_and_remove_client) {
	//Arrange
	const size_t count_of_operators = 3;
	const size_t max_waiting_queue_size = 3;
	std::pair<size_t, size_t> client_talking_min_max_time_range = std::make_pair(5u, 5u);
	std::pair<size_t, size_t> client_waiting_min_max_time_range = std::make_pair(5u, 5u);
	
	Operator operators(count_of_operators, max_waiting_queue_size,
										 client_talking_min_max_time_range,
										 client_waiting_min_max_time_range);
	
	const std::string client_id = "ID##10";
	const size_t expected_count = 1;
	
	//Act
	operators.add_client("ID##1");
	operators.add_client(client_id);
	operators.add_client("ID##5");
	operators.find_and_remove_client(client_id);
	
	//Assert
	ASSERT_EQ(expected_count, operators.count_of_free_operators());
}


TEST(OperatorFixture, find_client_position) {
	//Arrange
	const size_t count_of_operators = 3;
	const size_t max_waiting_queue_size = 3;
	std::pair<size_t, size_t> client_talking_min_max_time_range = std::make_pair(5u, 5u);
	std::pair<size_t, size_t> client_waiting_min_max_time_range = std::make_pair(5u, 5u);
	
	Operator operators(count_of_operators, max_waiting_queue_size,
										 client_talking_min_max_time_range,
										 client_waiting_min_max_time_range);
	
	const std::string client_id = "ID##10";
	const size_t expected_count = 3;
	
	//Act
	operators.add_waiting_client("ID##1");
	operators.add_waiting_client("ID##2");
	operators.add_waiting_client(client_id);
	
	//Assert
	ASSERT_EQ(expected_count, operators.find_client_position(client_id));
}


TEST(OperatorFixture, check_for_unique_phone_number) {
	//Arrange
	const size_t count_of_operators = 3;
	const size_t max_waiting_queue_size = 3;
	std::pair<size_t, size_t> client_talking_min_max_time_range = std::make_pair(5u, 5u);
	std::pair<size_t, size_t> client_waiting_min_max_time_range = std::make_pair(5u, 5u);
	
	Operator operators(count_of_operators, max_waiting_queue_size,
										 client_talking_min_max_time_range,
										 client_waiting_min_max_time_range);
	
	const std::string phone_number = "123456";
	
	//Act
	operators.set_client_phone_number("456789");
	operators.set_client_phone_number("798789");
	operators.set_client_phone_number(phone_number);
	
	//Assert
	ASSERT_FALSE(operators.check_for_unique_phone_number(phone_number));
	ASSERT_TRUE(operators.check_for_unique_phone_number("654654"));
}



int main(int argc, char* argv[]) {
	::testing::InitGoogleTest(&argc, argv);
	std::ignore = RUN_ALL_TESTS();
	return 0;
}
