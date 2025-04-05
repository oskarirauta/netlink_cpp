#include <algorithm>
#include <utility>

#include "netlink/types.hpp"

nl::prefix& nl::prefix::operator =(int length) {

	nl::prefix other(length);
	this -> _length = other._length;
	return *this;
}

nl::prefix& nl::prefix::operator =(const nl::prefix& other) {

	if ( other._length < 1 || other._length > 32 )
		return *this;

	this -> _length = other._length;
	return *this;
}

bool nl::prefix::operator ==(const nl::prefix& other) const {

	return this -> _length == other._length;
}

bool nl::prefix::operator ==(int length) const {

	return this -> _length == length;
}

nl::prefix::operator std::string() const {

	return std::to_string(this -> _length);
}

nl::prefix::operator int() const {

	return this -> _length;
}

nl::prefix::operator __u8() const {

	return (__u8)this -> _length;
}

nl::ipaddress nl::prefix::netmask() const {

	int ip = 0xffffffffu << (32 - this -> _length);
	unsigned char mask[4] = {
		(unsigned char)( ip >> 24 ), (unsigned char)( ip >> 16 & 0xff ), (unsigned char)( ip >> 8 & 0xff ), (unsigned char)( ip >> 0 & 0xff )
        };

	std::string s =
		std::to_string((int)mask[0]) + "." +
		std::to_string((int)mask[1]) + "." +
		std::to_string((int)mask[2]) + "." +
		std::to_string((int)mask[3]);

	return nl::ipaddress(s);
}

nl::ipaddress nl::prefix::broadcast(const nl::ipaddress& addr) const {

	return addr.broadcast(*this);
}

bool nl::prefix::valid() const {

	return this -> _length >= 1 && this -> _length <= 32;
}

std::string nl::prefix::to_string() const {

	return this -> operator std::string();
}

nl::prefix::prefix(int length) {

	this -> _length = length >= 1 && length <= 32 ? length : 24;
}

nl::prefix::prefix(const nl::ipaddress& addr) {

	int cidr = 0;
	std::vector<std::pair<int, int>> modifier = {
		{ 0x80, 1 }, { 0xC0, 2 }, { 0xE0, 3 }, { 0xF0, 4 },
		{ 0xF8, 5 }, { 0xFC, 6 }, { 0xFE, 7 }, { 0xFF, 8 }
	};

	for ( const auto& c : addr.components()) {
		if ( auto it = std::find_if(modifier.begin(), modifier.end(), [&c](const std::pair<int, int>& p) { return c == p.first; });
			it != modifier.end())
			cidr += it -> second;
	}

	this -> _length = cidr;
}

std::ostream& operator <<(std::ostream& os, const nl::prefix& prefix) {

	os << prefix.to_string();
	return os;
}
