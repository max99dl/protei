#include<iostream>
#include<fstream>
#include<vector>
#include<algorithm>


int main() {
	std::vector<int> vector {1, 2, 3, 4, 5, 6};
	auto position = std::find(vector.begin(), vector.end(), 5);
	std::cout << position - vector.begin() + 1 << std::endl;
	return 0;
}
