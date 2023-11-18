#!/bin/bash

rm -r build;
cmake -Bbuild > empty.txt;
cmake --build build > empty.txt;
rm empty.txt
./build/main
