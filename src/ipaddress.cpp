#include <algorithm>
#include <stdexcept>
#include <cstring>
#include <arpa/inet.h>

#include "netlink/types.hpp"

nl::ipaddress& nl::ipaddress::operator =(const nl::ipaddress& other) {

	this -> _addr = other._addr;
	return *this;
}

nl::ipaddress& nl::ipaddress::operator =(const std::string& addr) {

	nl::ipaddress other(addr);
	this -> _addr = other._addr;
	return *this;
}

bool nl::ipaddress::operator ==(const char* addr) const {

	return this -> _addr == std::string(addr);
}

bool nl::ipaddress::operator ==(const std::string& addr) const {

	return this -> _addr == addr;
}

bool nl::ipaddress::operator ==(const nl::ipaddress& other) const {

	return this -> _addr == other._addr;
}

nl::ipaddress::operator std::string() const {

	return this -> _addr;
}

nl::ipaddress::operator in_addr() const {

	in_addr addr;
	if ( this -> _addr.empty() || ::inet_pton(AF_INET, this -> _addr.c_str(), &addr) < 0 )
		throw std::runtime_error("failed to convert ip address " + this -> _addr + " to binary format, address is not valid");

	return addr;
}

std::vector<int> nl::ipaddress::components() const {

	if ( this -> _addr.empty() || std::count_if(this -> _addr.begin(), this -> _addr.end(),
		[](char ch) { return ch == '.'; }) != 3 ) return {};

	std::string str(this -> _addr);
	std::vector<std::string> vec;
	size_t pos;

	while (( pos = str.find_first_of('.'))  != std::string::npos ) {

		vec.push_back(str.substr(0, pos));
		str.erase(0, pos + 1);
	}

	if ( !str.empty())
		vec.push_back(str);

	if ( vec.size() != 4 ||
		vec[0].empty() || vec[0].size() > 3 ||
		vec[1].empty() || vec[1].size() > 3 ||
		vec[2].empty() || vec[2].size() > 3 ||
		vec[3].empty() || vec[3].size() > 3 ||
		vec[0].find_first_not_of("1234567890") != std::string::npos ||
		vec[1].find_first_not_of("1234567890") != std::string::npos ||
		vec[2].find_first_not_of("1234567890") != std::string::npos ||
		vec[3].find_first_not_of("1234567890") != std::string::npos )
		return {};

	std::vector<int> comps;

	for ( const std::string& s : vec ) {

		try {
			int n = std::stoi(s);

			// addressed beginning with 0 are not valid ip addresses
			if ( n < 0 || n > 255 || ( comps.empty() && n == 0 ))
				return {};

			comps.push_back(n);
		} catch (...) {
			return {};
		}
	}

	return comps;
}

bool nl::ipaddress::valid() const {

	if ( this -> _addr.empty())
		return false;

	std::vector<int> comps = this -> components();

	if ( comps.size() != 4 || comps[0] < 0 || comps[0] > 255 || comps[1] < 0 || comps[1] > 255 ||
		comps[2] < 0 || comps[2] > 255 || comps[3] < 0 || comps[3] > 255 )
		return false;

	return true;
}

bool nl::ipaddress::empty() const {

	if ( this -> _addr.empty() || !this -> valid())
		return true;

	std::vector<int> comps = this -> components();
	return comps.size() != 4 || ( comps[0] == 0 && comps[1] == 0 && comps[2] == 0 && comps[3] == 0 );
}

nl::ipaddress nl::ipaddress::broadcast(const nl::prefix& prefix) const {

	in_addr mask, broadcast;

	std::memset(&mask, 0, sizeof(mask));
	mask.s_addr = ::htonl((int)prefix == 0 ? 0 : ( ~((1 << (32 - (int)prefix)) - 1)));

	std::memset(&broadcast, 0, sizeof(broadcast));
	broadcast.s_addr = (this -> to_in_addr().s_addr & mask.s_addr) | ~mask.s_addr;

	char buf[INET_ADDRSTRLEN];
	std::string bcast = "0.0.0.0";
	if ( ::inet_ntop(AF_INET, &broadcast.s_addr, buf, sizeof(buf)) != nullptr )
		bcast = std::string(buf);

	return nl::ipaddress(bcast);
}

std::string nl::ipaddress::to_string() const {

	return this -> operator std::string();
}

in_addr nl::ipaddress::to_in_addr() const {

	return this -> operator in_addr();
}

nl::ipaddress::ipaddress(const std::string& addr) {

	this -> _addr = nl::ipaddress::sanitaze(addr);
}

nl::ipaddress::ipaddress(const nl::prefix& prefix) {

	nl::ipaddress ip = prefix.netmask();
	this -> _addr = ip._addr;
}

std::string nl::ipaddress::sanitaze(const std::string& addr) {

	std::string _addr(addr);
	std::vector<std::string> vec;
	size_t pos;

	if ( _addr.empty())
		return "0.0.0.0";

	while (( pos = _addr.find_first_of('.')) != std::string::npos ) {

		vec.push_back(_addr.substr(0, pos));
		_addr.erase(0, pos + 1);
	}

	if ( !_addr.empty())
		vec.push_back(_addr);

	if ( vec.size() != 4 || vec.at(3).empty() ||
		std::find_if(vec.begin(), vec.end(), [](const std::string& s) {

			for ( const auto& ch : s )
				if ( !::isdigit(ch))
					return true;

			if ( int i = std::atoi(s.c_str()); i < 0 || i > 255 )
				return true;

			return false;

		}) != vec.end())
		return "0.0.0.0";

	return vec[0] + "." + vec[1] + "." + vec[2] + "." + vec[3];
}

std::ostream& operator <<(std::ostream& os, const nl::ipaddress& ipaddress) {

	os << ipaddress.to_string();
	return os;
}
