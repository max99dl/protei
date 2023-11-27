cmake_minimum_required(VERSION 3.15)

project(WebServer_library)


add_library(web_server_lib STATIC src/web_server.cpp src/web_server.hpp)
set_target_properties(
	web_server_lib PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON 
)

target_compile_options(
	web_server_lib PRIVATE
#	-I"src/spdlog/include"
	-Wall
)

target_include_directories(
	web_server_lib
	PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/spdlog/include
)
target_include_directories(
	web_server_lib
	PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/boost
)

install(
	TARGETS web_server_lib EXPORT web_server_libTargets
	LIBRARY DESTINATION lib
	#ARCHIVE fro static libraries
	ARCHIVE DESTINATION lib
	INCLUDES DESTINATION include
)
install(FILES src/web_server.hpp DESTINATION include)

install(
	EXPORT web_server_libTargets
	FILE web_server_libConfig.cmake
	DESTINATION lib/cmake
)
