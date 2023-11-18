cmake_minimum_required(VERSION 3.15)

project(Main)


set(SOURCES
	main.cpp
	Client.cpp
	Operator.cpp
)


set(HEADERS
	Client.hpp
	Operator.hpp
)

#main server
add_executable(main ${SOURCES} ${HEADERS})
set_target_properties(
	main PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON
)


#unit-tests
add_executable(tests
	tests.cpp
	Client.cpp
	Operator.cpp
	${HEADERS}
)
set_target_properties(
	tests PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON
)
