#include<iostream>
#include<fstream>


int main() {
	std::ofstream file("example.txt", std::ios::app);
	if(file) {
		file << "hello there";
	}
	return 0;
}
