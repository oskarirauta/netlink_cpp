#include <stdexcept>
#include "netlink/attr.hpp"

nl::attr& nl::attr::operator =(const int& n) {
	this -> emplace<int>(std::forward<decltype(n)>(n));
	this -> _size = sizeof(__s32);
	return *this;
}

nl::attr& nl::attr::operator =(const __u8& n) {
	this -> emplace<__u8>(std::forward<decltype(n)>(n));
	this -> _size = sizeof(__u8);
	return *this;
}

nl::attr& nl::attr::operator =(const __u16& n) {
	this -> emplace<__u16>(std::forward<decltype(n)>(n));
	this -> _size = sizeof(__u16);
	return *this;
}

nl::attr& nl::attr::operator =(const __u64& n) {
	this -> emplace<__u64>(std::forward<decltype(n)>(n));
	this -> _size = sizeof(__u64);
	return *this;
}

nl::attr& nl::attr::operator =(const nl::ipaddress& addr) {
	this -> emplace<nl::ipaddress>(std::forward<decltype(addr)>(addr));
	this -> _size = sizeof(in_addr);
	return *this;
}

nl::attr& nl::attr::operator =(const nl::interface& ifd) {
	this -> emplace<nl::interface>(std::forward<decltype(ifd)>(ifd));
	this -> _size = ((std::string)ifd).size() + 1;
	return *this;
}

nl::attr& nl::attr::operator =(const nl::prefix& prefix) {
	this -> emplace<nl::prefix>(std::forward<decltype(prefix)>(prefix));
	this -> _size = sizeof(__u8);
	return *this;
}

nl::attr& nl::attr::operator =(const nl::netns& netns) {
	this -> emplace<nl::netns>(std::forward<decltype(netns)>(netns));
	this -> _size = sizeof(__s32);
	return *this;
}

nl::attr& nl::attr::operator =(const std::string& s) {
	this -> emplace<std::string>(std::forward<decltype(s)>(s));
	this -> _size = s.size() + 1;
	return *this;
}

nl::attr& nl::attr::operator =(const std::nullptr_t& n) {
	this -> emplace<std::nullptr_t>(std::forward<decltype(n)>(n));
	this -> _size = 0;
	return *this;
}

nl::attr::operator int() const {
        try { return this -> to_int(); } catch ( const std::runtime_error& e ) { throw e; }
}

nl::attr::operator __u8() const {
        try { return this -> to_u8(); } catch ( const std::runtime_error& e ) { throw e; }
}

nl::attr::operator __u16() const {
        try { return this -> to_u16(); } catch ( const std::runtime_error& e ) { throw e; }
}

nl::attr::operator __u64() const {
        try { return this -> to_u64(); } catch ( const std::runtime_error& e ) { throw e; }
}

nl::attr::operator std::string() const {
        try { return this -> to_string(); } catch ( const std::runtime_error& e ) { throw e; }
}

nl::attr::operator nl::ipaddress() const {
	try { return this -> to_ipaddress(); } catch ( const std::runtime_error& e ) { throw e; }
}

nl::attr::operator nl::interface() const {
	try { return this -> to_interface(); } catch ( const std::runtime_error& e ) { throw e; }
}

nl::attr::operator nl::prefix() const {
	try { return this -> to_prefix(); } catch ( const std::runtime_error& e ) { throw e; }
}

nl::attr::operator nl::netns() const {
	try { return this -> to_netns(); } catch ( const std::runtime_error& e ) { throw e; }
}

int nl::attr::to_int() const {

	if ( std::holds_alternative<int>(*this))
		return std::get<int>(*this);
	else if ( std::holds_alternative<__u8>(*this))
		return (int)std::get<__u8>(*this);
	else if ( std::holds_alternative<__u16>(*this))
		return (int)std::get<__u16>(*this);
	else if ( std::holds_alternative<__u64>(*this))
		return (int)std::get<__u64>(*this);

	throw std::runtime_error("cannot typecast attribute to int");
}

__u8 nl::attr::to_u8() const {

	if ( std::holds_alternative<int>(*this))
		return (__u8)std::get<int>(*this);
	else if ( std::holds_alternative<__u8>(*this))
		return std::get<__u8>(*this);
	else if ( std::holds_alternative<__u16>(*this))
		return (__u8)std::get<__u16>(*this);
	else if ( std::holds_alternative<__u64>(*this))
		return (__u64)std::get<__u64>(*this);

	throw std::runtime_error("cannot typecast attribute to __u8");
}

__u16 nl::attr::to_u16() const {

	if ( std::holds_alternative<int>(*this))
		return (__u16)std::get<int>(*this);
	else if ( std::holds_alternative<__u8>(*this))
		return (__u16)std::get<__u8>(*this);
	else if ( std::holds_alternative<__u16>(*this))
		return std::get<__u16>(*this);
	else if ( std::holds_alternative<__u64>(*this))
		return (__u16)std::get<__u64>(*this);

	throw std::runtime_error("cannot typecast attribute to __u16");
}

__u64 nl::attr::to_u64() const {

	if ( std::holds_alternative<int>(*this))
		return (__u64)std::get<int>(*this);
	else if ( std::holds_alternative<__u8>(*this))
		return (__u64)std::get<__u8>(*this);
	else if ( std::holds_alternative<__u16>(*this))
		return (__u64)std::get<__u16>(*this);
	else if ( std::holds_alternative<__u64>(*this))
		return std::get<__u64>(*this);

	throw std::runtime_error("cannot typecast attribute to __u64");
}

nl::ipaddress nl::attr::to_ipaddress() const {

	if ( std::holds_alternative<nl::ipaddress>(*this))
		return std::get<nl::ipaddress>(*this);

	throw std::runtime_error("cannot typecast attribute to ipaddress");
}

nl::interface nl::attr::to_interface() const {

	if ( std::holds_alternative<nl::interface>(*this))
		return std::get<nl::interface>(*this);

	throw std::runtime_error("cannot typecast attribute to interface");
}

nl::prefix nl::attr::to_prefix() const {

	if ( std::holds_alternative<nl::prefix>(*this))
		return std::get<nl::prefix>(*this);

	throw std::runtime_error("cannot typecast attribute to prefix");
}

nl::netns nl::attr::to_netns() const {

	if ( std::holds_alternative<nl::netns>(*this))
		return std::get<nl::netns>(*this);

	throw std::runtime_error("cannot typecast attribute to netns");
}

std::string nl::attr::to_string() const {

	if ( std::holds_alternative<std::string>(*this))
		return std::get<std::string>(*this);
	else if ( std::holds_alternative<int>(*this))
		return std::to_string(std::get<int>(*this));
	else if ( std::holds_alternative<__u8>(*this))
		return std::to_string(std::get<__u8>(*this));
	else if ( std::holds_alternative<__u16>(*this))
		return std::to_string(std::get<__u16>(*this));
	else if ( std::holds_alternative<__u64>(*this))
		return std::to_string(std::get<__u64>(*this));

	throw std::runtime_error("cannot typecast or convert attribute to string");
}

void* nl::attr::to_data() {

	if ( std::holds_alternative<void*>(*this)) {
	}

	throw std::runtime_error("cannot typecast attribute to void*");
}

bool nl::attr::is_int() const {
	return std::holds_alternative<int>(*this);
}

bool nl::attr::is_u8() const {
	return std::holds_alternative<__u8>(*this);
}

bool nl::attr::is_u16() const {
	return std::holds_alternative<__u16>(*this);
}

bool nl::attr::is_u64() const {
	return std::holds_alternative<__u64>(*this);
}

bool nl::attr::is_string() const {
	return std::holds_alternative<std::string>(*this);
}

bool nl::attr::is_data() const {
	return std::holds_alternative<void*>(*this);
}

bool nl::attr::is_null() const {
	return std::holds_alternative<std::nullptr_t>(*this);
}

bool nl::attr::is_ipaddress() const {
	return std::holds_alternative<nl::ipaddress>(*this);
}

bool nl::attr::is_prefix() const {
	return std::holds_alternative<nl::prefix>(*this);
}

bool nl::attr::is_netns() const {
	return std::holds_alternative<nl::netns>(*this);
}

bool nl::attr::is_interface() const {
	return std::holds_alternative<nl::interface>(*this);
}

bool nl::attr::is_number() const {
	return this -> is_int() || this -> is_u8() || this -> is_u16() || this -> is_u64();
}

size_t nl::attr::size() const {
	return this -> _size < 0 ? this -> fix_size() : (size_t)this -> _size;
}

nl::attr& nl::attr::set_size(const size_t& sz) {
	this -> _size = sz < 0 ? 0 : sz;
	return *this;
}

nl::attr nl::attr::padding(const size_t& sz) {

	return nl::attr(nullptr).set_size(sz);
}

size_t nl::attr::fix_size() const {

	if ( std::holds_alternative<int>(*this))
		return sizeof(__s32);
	else if ( std::holds_alternative<__u8>(*this))
		return sizeof(__u8);
	else if ( std::holds_alternative<__u16>(*this))
		return sizeof(__u16);
	else if ( std::holds_alternative<__u64>(*this))
		return sizeof(__u64);
	else if ( std::holds_alternative<nl::ipaddress>(*this))
		return sizeof(in_addr);
	else if ( std::holds_alternative<nl::interface>(*this))
		return ((std::string)std::get<nl::interface>(*this)).size() + 1;
	else if ( std::holds_alternative<nl::prefix>(*this))
		return sizeof(__u8);
	else if ( std::holds_alternative<std::string>(*this))
		return std::get<std::string>(*this).size() + 1;
	else if ( std::holds_alternative<void*>(*this))
		return 0;
	else if ( std::holds_alternative<std::nullptr_t>(*this))
		return 0;

	return 0;
}
