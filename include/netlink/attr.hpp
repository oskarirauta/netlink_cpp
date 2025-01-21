#pragma once

#include <string>
#include <variant>
#include <vector>
#include <linux/types.h>
#include "netlink/types.hpp"
#include "netlink/netns.hpp"

namespace nl {

	class attr : public std::variant<int, __u8, __u16, __u64, nl::ipaddress, nl::interface, nl::prefix, nl::netns, std::string, void*, std::nullptr_t> {

		public:

			using variant::variant;

			attr& operator =(const int& n);
			attr& operator =(const __u8& n);
			attr& operator =(const __u16& n);
			attr& operator =(const __u64& n);
			attr& operator =(const nl::ipaddress& addr);
			attr& operator =(const nl::interface& ifd);
			attr& operator =(const nl::prefix& prefix);
			attr& operator =(const nl::netns& netns);
			attr& operator =(const std::string& s);
			attr& operator =(const std::nullptr_t& n);

			operator int() const;
			operator __u8() const;
			operator __u16() const;
			operator __u64() const;
			operator std::string() const;
			operator nl::ipaddress() const;
			operator nl::interface() const;
			operator nl::prefix() const;
			operator nl::netns() const;

			int to_int() const;
			__u8 to_u8() const;
			__u16 to_u16() const;
			__u64 to_u64() const;
			std::string to_string() const;
			nl::ipaddress to_ipaddress() const;
			nl::interface to_interface() const;
			nl::prefix to_prefix() const;
			nl::netns to_netns() const;
			void* to_data();

			bool is_int() const;
			bool is_u8() const;
			bool is_u16() const;
			bool is_u64() const;
			bool is_ipaddress() const;
			bool is_interface() const;
			bool is_prefix() const;
			bool is_netns() const;
			bool is_string() const;
			bool is_data() const;
			bool is_null() const;

			bool is_number() const;

			size_t size() const;
			attr& set_size(const size_t& sz); // for void* initializer

			attr(void* data, size_t sz) : std::variant<
				int, __u8, __u16, __u64,
				nl::ipaddress, nl::interface, nl::prefix, nl::netns,
				std::string, void*, std::nullptr_t>(data), _size(sz) {}

			static attr padding(const size_t& sz);

		private:

			long _size = -1;
			size_t fix_size() const;

	};

}
