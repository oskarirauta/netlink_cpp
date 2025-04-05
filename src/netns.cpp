#include <stdexcept>
#include <unistd.h>
#include <fcntl.h>

#include "netlink/netns.hpp"

static bool fd_is_open(int fd) {

        return ::fcntl(fd, F_GETFD) != -1 || errno != EBADF;
}

static bool file_exists(const std::string& filename) {

	return ::access(filename.c_str(), F_OK) == 0;
}

static bool file_is_accessible(const std::string& filename) {

	return ::access(filename.c_str(), R_OK) == 0;
}

void nl::netns::close() {

	if ( this -> _fd < 0 )
		return;
	else if ( fd_is_open(this -> _fd))
		::close(this -> _fd);

	this -> _fd = -1;
}

bool nl::netns::is_open() {

	if ( this -> _fd >= 0 && !fd_is_open(this -> _fd))
		this -> _fd = -1;

	return this -> _fd >= 0;
}

bool nl::netns::is_open() const {

	return this -> _fd >= 0 && fd_is_open(this -> _fd);
}

void nl::netns::open() {

	if ( this -> _filename.empty())
		throw std::runtime_error("cannot open net namespace file, filename not specified");
	else if ( !file_exists(this -> _filename))
		throw std::runtime_error("cannot open net namespace file " + this -> _filename + ", file does not exist");
	else if ( !file_is_accessible(this -> _filename))
		throw std::runtime_error("cannot open net namespace file "  + this -> _filename + ", file not readable or no permission");

	if ( this -> _fd = ::open(this -> _filename.c_str(), O_RDONLY); this -> _fd < 0 ) {
		this -> _fd = -1;
		throw std::runtime_error("failed to open net namespace file " + this -> _filename);
	}
}

nl::netns& nl::netns::operator =(const nl::netns& other) {

	this -> _filename = other._filename;
	this -> _fd = other._fd;
	return *this;
}

bool nl::netns::operator ==(const nl::netns& other) const {

	return this -> _filename == other._filename && this -> _fd == other._fd;
}

bool nl::netns::operator ==(const std::string& filename) const {

	return this -> _filename == filename;
}

bool nl::netns::operator ==(int fd) const {

	return this -> _fd == fd;
}

nl::netns::operator std::string() const {

	return this -> _filename;
}

nl::netns::operator int() const {

	return this -> _fd;
}

std::string nl::netns::filename() const {

	return this -> operator std::string();
}

int nl::netns::fd() const {

	return this -> operator int();
}

nl::netns::netns(pid_t pid) {

	this -> _filename = "/proc/" + std::to_string(pid) + "/ns/net";
}

nl::netns::~netns() {

	this -> close();
}

std::ostream& operator <<(std::ostream& os, const nl::netns& netns) {

	os << (std::string)netns;
	return os;
}
