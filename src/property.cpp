#include "netlink/property.hpp"

int nl::property::type() {
	return this -> first;
}

nl::attr& nl::property::value() {
	return this -> second;
}

nl::property& nl::property::operator =(const nl::attr& attr) {
	this -> second = attr;
	return *this;
}
