#include <net/if.h>
#include "netlink/types.hpp"

nl::interface& nl::interface::operator =(const std::string& ifd) {

	nl::interface other(ifd);
	this -> _ifd = other._ifd;
	return *this;
}

nl::interface& nl::interface::operator =(const nl::interface& other) {

	this -> _ifd = other._ifd;
	return *this;
}

bool nl::interface::operator ==(const char* ifd) const {

	return this -> _ifd == std::string(ifd);
}

bool nl::interface::operator ==(const std::string& ifd) const {

	return this -> _ifd == ifd;
}

bool nl::interface::operator ==(const nl::interface& other) const {

	return this -> _ifd == other._ifd;
}

nl::interface::operator std::string() const {

	return this -> _ifd;
}

nl::interface::operator int() const {

	return this -> get_index();
}

nl::interface::operator unsigned int() const {

	return (unsigned int)this -> get_index();
}

bool nl::interface::empty() const {

	return this -> _ifd.empty();
}

bool nl::interface::exists() const {

	if ( this -> _ifd.empty())
		return false;

	return this -> get_index() > 0;
}

std::string nl::interface::to_string() const {

	return this -> operator std::string();
}

int nl::interface::to_if_index() const {

	return this -> operator int();
}

int nl::interface::get_index() const {

	if ( this -> _ifd.empty())
		return 0;

	return ::if_nametoindex(this -> _ifd.c_str());
}

std::ostream& operator <<(std::ostream& os, const nl::interface& interface) {

	os << interface.to_string();
	return os;
}
