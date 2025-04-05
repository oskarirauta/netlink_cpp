#include <linux/if.h>
#include "netlink/msg.hpp"

nl::msg nl::msg::add_interface(const nl::interface& ifd, const std::string& kind) {

	return {
		.type = RTM_NEWLINK, .flags = NLM_F_REQUEST | NLM_F_CREATE | NLM_F_EXCL | NLM_F_ACK,
		.data = (ifinfomsg) {
			.ifi_family = AF_UNSPEC
		},
		.properties = {{ IFLA_IFNAME, ifd.to_string() }},
		.nested = {
			{ IFLA_LINKINFO, {{ IFLA_INFO_KIND, nl::attr(kind) }}},
			{ IFLA_INFO_DATA, {{ -1, nullptr }}},
		},
	};
}

nl::msg nl::msg::create_veth_pair(const nl::interface& veth, const nl::interface& peer) {

	return {
		.type = RTM_NEWLINK, .flags = NLM_F_REQUEST | NLM_F_CREATE | NLM_F_EXCL | NLM_F_ACK,
		.data = (ifinfomsg) {
			.ifi_family = AF_UNSPEC
		},
		.properties = {{ IFLA_IFNAME, veth.to_string() }},
		.nested = {
			{ IFLA_LINKINFO, {{ IFLA_INFO_KIND, "veth" }}},
			{ IFLA_INFO_DATA, {{ -1, nullptr }}},
			{ VETH_INFO_PEER, {
				{ -1, nl::attr::padding(sizeof(ifinfomsg)) },
				{ IFLA_IFNAME, peer.to_string() },
			}},
		},
	};
}

nl::msg nl::msg::add_ip_address(const nl::interface& ifd, const nl::ipaddress& addr, const nl::prefix& prefix) {

	return {
                .type = RTM_NEWADDR, .flags = NLM_F_ACK | NLM_F_REQUEST | NLM_F_CREATE | NLM_F_EXCL,
                .data = (ifaddrmsg) {
                        .ifa_family = AF_INET,
                        .ifa_prefixlen = (__u8)prefix,
                        .ifa_scope = 0,
                        .ifa_index = (__u32)ifd.to_if_index(),
                },
                .properties = {
                        { IFA_LOCAL, addr },
                        { IFA_ADDRESS, addr },
                        { IFA_BROADCAST, addr.broadcast(prefix) },
                },
        };
}

nl::msg nl::msg::bring_up(const nl::interface& ifd) {

	return {
                .type = RTM_NEWLINK, .flags = NLM_F_ACK | NLM_F_REQUEST,
                .data = (ifinfomsg) {
                        .ifi_family = AF_UNSPEC,
                        .ifi_index = ifd.to_if_index(),
                        .ifi_flags = 0 | IFF_UP,
                        .ifi_change = 0 | IFF_UP,
                },
        };
}

nl::msg nl::msg::set_master(const nl::interface& ifd, const nl::interface& master_ifd) {

	return {
		.type = RTM_NEWLINK, .flags = NLM_F_ACK | NLM_F_REQUEST,
		.data = (ifinfomsg) {
			.ifi_family = AF_UNSPEC,
			.ifi_index = (int)ifd.to_if_index(),
		},
		.properties = {
			{ IFLA_MASTER, (int)master_ifd.to_if_index(), }
		},
	};
}

nl::msg nl::msg::bring_down(const nl::interface& ifd) {

	return {
		.type = RTM_NEWLINK, .flags = NLM_F_ACK | NLM_F_REQUEST,
		.data = (ifinfomsg) {
			.ifi_family = AF_UNSPEC,
			.ifi_index = ifd.to_if_index(),
			.ifi_flags = 0 & ~IFF_UP,
			.ifi_change = 0 | IFF_UP,
		},
	};
}

nl::msg nl::msg::delete_link(const nl::interface& ifd) {

	return {
		.type = RTM_DELLINK, .flags = NLM_F_ACK | NLM_F_REQUEST,
		.data = (ifinfomsg) {
			.ifi_family = AF_UNSPEC
		},
		.properties = {{ IFLA_IFNAME, ifd.to_string() }},
	};
}

nl::msg nl::msg::set_mtu(const nl::interface& ifd, int mtu) {

	return {
		.type = RTM_NEWLINK, .flags = NLM_F_ACK | NLM_F_REQUEST,
		.data = (ifinfomsg) {
			.ifi_family = AF_UNSPEC,
		},
		.properties = {
			{ IFLA_IFNAME, ifd.to_string() },
			{ IFLA_MTU, mtu },
		},
	};
}

nl::msg nl::msg::join_ns(const nl::interface& ifd, const nl::netns& netns) {

	return {
		.type = RTM_NEWLINK, .flags = NLM_F_ACK | NLM_F_REQUEST,
		.data = (ifinfomsg) {
			.ifi_family = AF_UNSPEC,
			.ifi_index = ifd.to_if_index(),
		},
		.properties = {
			{ IFLA_NET_NS_FD, netns.fd() },
		},
	};
}

nl::msg nl::msg::rename_interface(const nl::interface& ifd, const nl::interface& new_ifd) {

	return {
		.type = RTM_NEWLINK, .flags = NLM_F_ACK | NLM_F_REQUEST,
		.data = (ifinfomsg) {
			.ifi_family = AF_UNSPEC,
			.ifi_index = ifd.to_if_index(),
		},
		.properties = {
			{ IFLA_IFNAME, new_ifd.to_string() },
		},
	};
}

nl::msg nl::msg::add_route(const nl::ipaddress& addr) {

	return {
		.type = RTM_NEWROUTE, .flags = NLM_F_ACK | NLM_F_REQUEST | NLM_F_CREATE | NLM_F_EXCL,
		.data = (rtmsg) {
			.rtm_family = AF_INET,
			.rtm_dst_len = 0,
			.rtm_table = RT_TABLE_MAIN,
			.rtm_protocol = RTPROT_BOOT,
			.rtm_scope = RT_SCOPE_UNIVERSE,
			.rtm_type = RTN_UNICAST,
		},
		.properties = {
			{ RTA_GATEWAY, addr },
		},
	};
}

// get
