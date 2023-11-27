#!/bin/bash

mkdir server;
mv server_config.json server;

cd libraries;
mkdir build;
cd build;
cmake .. -DCMAKE_INSTALL_PREFIX=install;
cmake --build .;
cmake --build . --target install;

cd ../../tests/googletest;
cmake -Bbuild;
cmake --build build;

cd ../../;
mkdir build;
cd build;
cmake .. -Dweb_server_lib_DIR=./libraries/build/install/lib/cmake;
cmake --build .;
mv main ./../server;
mv tests ./../server;

cd ..;
