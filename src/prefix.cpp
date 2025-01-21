#include "netlink/types.hpp"
/*
nl::prefix& nl::prefix::operator =(int length) {

	if ( length < 1 || length > 32 )
		return *this;

	this -> _length = length;
	return *this;
}
*/
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

nl::ipaddress nl::prefix::mask(const nl::ipaddress& addr) const {

	return this -> valid() ? nl::ipaddress() : addr.mask(this -> _length);
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
