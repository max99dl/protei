cmake_minimum_required(VERSION 3.15)

project(Main)

if(NOT TARGET spdlog)
    find_package(spdlog REQUIRED)
endif()

#main server
add_executable(main src/main.cpp)
set_target_properties(
	main PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON
)
find_package(web_server_lib CONFIG REQUIRED)
target_link_libraries(main web_server_lib spdlog::spdlog_header_only)

