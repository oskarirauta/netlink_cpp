#include <iostream>
#include "netlink.hpp"

int main(int argc, char** argv) {


	auto vec = nl::network::interfaces();
	std::cout << vec << std::endl;
	return 0;
}
