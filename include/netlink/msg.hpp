 #pragma once

#include <variant>
#include <linux/types.h>
#include <linux/rtnetlink.h>
#include <linux/if_addr.h>

#include "netlink/types.hpp"
#include "netlink/property.hpp"

namespace nl {

	struct msg {

		public:

			int type;
			int flags = 0;
			std::variant<rtmsg, ifinfomsg, prefixmsg, ifaddrmsg> data;
			std::vector<nl::property> properties;
			std::vector<std::pair<int,std::vector<nl::property>>> nested;

			static nl::msg add_interface(const nl::interface& ifd, const std::string& kind);

			static msg create_veth_pair(const nl::interface& veth, const nl::interface& peer);
			static msg add_ip_address(const nl::interface& ifd, const nl::ipaddress& addr, const nl::prefix& prefix);
			static msg bring_up(const nl::interface& ifd);
			static nl::msg set_master(const nl::interface& ifd, const nl::interface& master_ifd);

			static nl::msg bring_down(const nl::interface& ifd);
			static nl::msg delete_link(const nl::interface& ifd);
			static nl::msg set_mtu(const nl::interface& ifd, int mtu);

			static nl::msg rename_interface(const nl::interface& ifd, const nl::interface& new_ifd);

			static nl::msg join_ns(const nl::interface& ifd, const nl::netns& netns);
			static nl::msg add_route(const nl::ipaddress& addr);

	};

}
