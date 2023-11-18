cmake_minimum_required(VERSION 3.15)

project(Client)

#for client
add_library(client_lib Client.cpp Client.hpp)
set_target_properties(
	client_lib PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON 
)

install(
	TARGETS client_lib EXPORT client_libTargets
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	INCLUDES DESTINATION include
)
install(FILES Client.hpp DESTINATION include)

install(
	EXPORT client_libTargets
	FILE client_libConfig.cmake
	DESTINATION lib/cmake
)



#for operator
add_library(operator_lib Operator.cpp Operator.hpp)
set_target_properties(
	operator_lib PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON 
)

install(
	TARGETS operator_lib EXPORT client_libTargets
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	INCLUDES DESTINATION include
)
install(FILES Client.hpp DESTINATION include)

install(
	EXPORT client_libTargets
	FILE client_libConfig.cmake
	DESTINATION lib/cmake
)
