#pragma once

#include <string>
#include <vector>

#include <netinet/in.h>
#include <linux/types.h>

extern const int VETH_INFO_PEER;

namespace nl {

	struct prefix;

	struct ipaddress {

		public:

			ipaddress& operator =(const std::string& addr);
			ipaddress& operator =(const ipaddress& other);

			bool operator ==(const char* other) const;
			bool operator ==(const std::string& other) const;
			bool operator ==(const ipaddress& other) const;
			operator std::string() const;
			operator in_addr() const;

			std::vector<int> components() const;
			bool valid() const;
			bool empty() const; // all zeros?
			ipaddress mask(const nl::prefix& prefix) const;
			ipaddress broadcast(const nl::prefix& prefix) const;
			std::string to_string() const;
			in_addr to_in_addr() const;

			ipaddress() : _addr("0.0.0.0") {}
			ipaddress(const std::string& addr);
			ipaddress(const nl::prefix& prefix);

		private:

			std::string _addr;
			static std::string sanitaze(const std::string& s);
	};

	struct prefix { // cidr

		public:

			prefix& operator =(int length);
			prefix& operator =(const prefix& other);

			bool operator ==(const prefix& other) const;
			bool operator ==(int length) const;

			operator std::string() const;
			operator int() const;
			operator __u8() const;

			nl::ipaddress netmask() const;
			nl::ipaddress broadcast(const nl::ipaddress& addr) const;
			bool valid() const;
			std::string to_string() const;

			prefix() : _length(24) {}
			prefix(int length);
			prefix(const nl::ipaddress& addr);

		private:

			int _length;
	};

	struct interface {

		public:

			interface& operator =(const std::string& ifd);
			interface& operator =(const interface& other);

			bool operator ==(const char* other) const;
			bool operator ==(const std::string& other) const;
			bool operator ==(const interface& other) const;

			operator std::string() const;
			operator int() const; // if index
			operator unsigned int() const; // if index

			bool empty() const;
			bool exists() const;
			std::string to_string() const;
			int to_if_index() const;

			interface() : _ifd("") {}
			interface(const std::string& ifd) : _ifd(ifd) {}

		private:

			std::string _ifd;
			int get_index() const;
	};

}

std::ostream& operator <<(std::ostream& os, const nl::ipaddress& ipaddress);
std::ostream& operator <<(std::ostream& os, const nl::prefix& prefix);
std::ostream& operator <<(std::ostream& os, const nl::interface& interface);
