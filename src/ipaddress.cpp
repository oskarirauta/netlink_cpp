#include <algorithm>
#include <stdexcept>
#include <arpa/inet.h>

#include "netlink/types.hpp"
/*
nl::ipaddress& nl::ipaddress::operator =(const std::string& addr) {

	this -> _addr = nl::ipaddress::sanitaze(addr);
	return *this;
}

nl::ipaddress& nl::ipaddress::operator =(const char* addr) {

	this -> _addr = nl::ipaddress::sanitaze(std::string(addr));
	return *this;
}
*/
nl::ipaddress& nl::ipaddress::operator =(const nl::ipaddress& other) {

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

	if ( !this -> _addr.empty() || !this -> valid())
		return true;

	std::vector<int> comps = this -> components();
	return comps.size() != 4 || ( comps[0] == 0 && comps[1] == 0 && comps[2] == 0 && comps[3] == 0 );
}

nl::ipaddress nl::ipaddress::mask(int prefix) const {

	if ( this -> empty() || prefix < 1 || prefix > 32 )
		return nl::ipaddress("0.0.0.0");

	std::vector<int> comps = this -> components();

	unsigned char mask[4];
	int ip = 0xffffffffu >> ( 32 - prefix ) << ( 32 - prefix );

	mask[3] = ( ip >> 0 ) && 0xff;
	mask[2] = ( ip >> 8 ) && 0xff;
	mask[1] = ( ip >> 16 ) && 0xff;
	mask[0] = ( ip >> 24 ) && 0xff;

	if ( mask[0] != 255 ) comps[0] = 255 - comps[0];
	if ( mask[1] != 255 ) comps[1] = 255 - comps[1];
	if ( mask[2] != 255 ) comps[2] = 255 - comps[2];
	if ( mask[3] != 255 ) comps[3] = 255 - comps[3];

	if ( comps[0] < 0 || comps[0] > 255 ||
		comps[1] < 0 || comps[1] > 255 ||
		comps[2] < 0 || comps[2] > 255 ||
		comps[3] < 0 || comps[3] > 255 )
		return nl::ipaddress("0.0.0.0");

	return nl::ipaddress(
		std::to_string(comps[0]) + std::string(".") +
		std::to_string(comps[1]) + std::string(".") +
		std::to_string(comps[2]) + std::string(".") +
		std::to_string(comps[3]));
}

nl::ipaddress nl::ipaddress::mask(const nl::prefix& prefix) const {

	int prefix_length = (int)prefix;
	return this -> mask(prefix_length);
}

std::string nl::ipaddress::to_string() const {

	return this -> operator std::string();
}

in_addr nl::ipaddress::to_in_addr() const {

	return this -> operator in_addr();
}

/*
nl::ipaddress::ipaddress(const char* addr) {

	this -> _addr = nl::ipaddress::sanitaze(std::string(addr));
}
*/
nl::ipaddress::ipaddress(const std::string& addr) {

	this -> _addr = nl::ipaddress::sanitaze(addr);
}

std::string nl::ipaddress::sanitaze(const std::string& addr) {

	std::vector<std::string> s = { "", "", "", "" };
	int n = 0;

	for ( auto ch : addr ) {

		if ( ch == '.' ) {
			if ( n >= 0 && n < 4 && s[n].empty())
				s[n] = "0";
			n++;
			continue;
		} else if ( ::isdigit(ch) && ch == '0' && s[n].empty())
			continue;
		else if ( ::isdigit(ch) && n >= 0 && n < 4 )
			s[n] += ch;
	}

	if ( n != 3 || s[3].empty())
		return "0.0.0.0";

	for ( int index = 0; index < 4; index++ ) {

		try {
			int i = std::atoi(s[index].c_str());
			if ( i < 0 || i > 255 || ( index == 0 && i == 0 ) || ( index == 3 && i == 0 ))
				return "0.0.0.0";
		} catch (...) { return "0.0.0.0"; }
	}

	return s[0] + "." + s[1] + "." + s[2] + "." + s[3];
}
