cmake_minimum_required(VERSION 3.15)

project(Main)

find_package(web_server_lib CONFIG REQUIRED)

#main server
add_executable(main src/main.cpp)
set_target_properties(
	main PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON
)

add_executable(tests tests/test_server.cpp)
set_target_properties(
	tests PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON
)

target_compile_options(
	main PRIVATE
	-Wall -pthread
)

target_compile_options(
	tests PRIVATE
	-Wall -pthread
)

target_include_directories(
	main
	PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/libraries/boost
	${CMAKE_CURRENT_SOURCE_DIR}/libraries/spdlog/include
)
target_link_libraries(main web_server_lib)

target_include_directories(
	tests
	PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/libraries/boost
	${CMAKE_CURRENT_SOURCE_DIR}/libraries/spdlog/include
	${CMAKE_CURRENT_SOURCE_DIR}/tests/googletest/build/lib
	${CMAKE_CURRENT_SOURCE_DIR}/tests/googletest/include
)
target_link_libraries(tests web_server_lib gtest gtest_main)
