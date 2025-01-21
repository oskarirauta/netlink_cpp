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

			//ipaddress& operator =(const char* addr);
			//ipaddress& operator =(const std::string& addr);
			ipaddress& operator =(const ipaddress& other);

			bool operator ==(const char* other) const;
			bool operator ==(const std::string& other) const;
			bool operator ==(const ipaddress& other) const;
			operator std::string() const;
			operator in_addr() const;

			std::vector<int> components() const;
			bool valid() const;
			bool empty() const; // all zeros?
			ipaddress mask(int prefix) const;
			ipaddress mask(const nl::prefix& prefix) const;
			std::string to_string() const;
			in_addr to_in_addr() const;

			ipaddress() : _addr("0.0.0.0") {}
			//ipaddress(const char* addr);
			ipaddress(const std::string& addr);

		private:

			std::string _addr;
			static std::string sanitaze(const std::string& s);
	};

	struct prefix { // cidr

		public:

			//prefix& operator =(int length);
			prefix& operator =(const prefix& other);

			bool operator ==(const prefix& other) const;
			bool operator ==(int length) const;

			operator std::string() const;
			operator int() const;
			operator __u8() const;

			nl::ipaddress mask(const nl::ipaddress& addr) const;
			bool valid() const;
			std::string to_string() const;

			prefix() : _length(24) {}
			prefix(int length);

		private:

			int _length;
	};

	struct interface {

		public:

			//interface& operator =(const char* ifd);
			//interface& operator =(const std::string& ifd);
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
			//interface(const char* ifd) : _ifd(std::string(ifd)) {}
			interface(const std::string& ifd) : _ifd(ifd) {}

		private:

			std::string _ifd;
			int get_index() const;
	};

}
