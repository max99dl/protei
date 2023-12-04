#!/bin/bash

mkdir server;
mv server_config.json server;
cp tests/test_server.cpp src/

cd libraries;
mkdir build;
cd build;
cmake .. -DCMAKE_INSTALL_PREFIX=install;
cmake --build .;
cmake --build . --target install;
cp install/lib/libweb_server_lib.a ../../src;
cp install/include/web_server.hpp ../../src;

cd ../../tests/googletest;
cmake -Bbuild;
cmake --build build;
cp ./build/lib/lib* ../../src/;

cd ../../src;
g++ -o tests -pthread -std=c++17 test_server.cpp -L. libweb_server_lib.a libgtest.a libgtest_main.a -I"../libraries/boost" -I"../libraries/spdlog/include" -I"../tests/googletest/include";
g++ -o main -pthread -std=c++17 main.cpp -L. libweb_server_lib.a -I"../libraries/spdlog/include" -I"../libraries/boost";
mv tests ../server;
mv main ../server;

rm test_server.cpp lib* web_server.hpp;
cd ..;
