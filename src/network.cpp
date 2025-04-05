#include <netdb.h>
#include <ifaddrs.h>
#include <linux/if_link.h>
#include <map>

#include "netlink/network.hpp"

#if !defined(IFF_LOWER_UP)
 #define IFF_LOWER_UP	1<<16
#endif

#if !defined(IFF_DORMANT)
 #define IFF_DORMANT	1<<17
#endif

#if !defined(IFF_ECHO)
 #define IFF_ECHO	1<<18
#endif

static std::map<nl::network::FLAG, std::string> names = {
	{ nl::network::FLAG::UP, "UP" },
	{ nl::network::FLAG::BROADCAST, "BROADCAST" },
	{ nl::network::FLAG::DEBUG, "DEBUG" },
	{ nl::network::FLAG::LOOPBACK, "LOOPBACK" },
	{ nl::network::FLAG::POINTOPOINT, "POINTOPOINT" },
	{ nl::network::FLAG::RUNNING, "RUNNING" },
	{ nl::network::FLAG::NOARP, "NOARP" },
	{ nl::network::FLAG::PROMISC, "PROMISC" },
	{ nl::network::FLAG::NOTRAILERS, "NOTRAILERS" },
	{ nl::network::FLAG::ALLMULTI, "ALLMULTI" },
	{ nl::network::FLAG::MASTER, "MASTER" },
	{ nl::network::FLAG::SLAVE, "SLAVE" },
	{ nl::network::FLAG::MULTICAST, "MULTICAST" },
	{ nl::network::FLAG::PORTSEL, "PORTSEL" },
	{ nl::network::FLAG::AUTOMEDIA, "AUTOMEDIA" },
	{ nl::network::FLAG::DYNAMIC, "DYNAMIC" },
	{ nl::network::FLAG::LOWER_UP, "LOWER_UP" },
	{ nl::network::FLAG::DORMANT, "DORMANT" },
	{ nl::network::FLAG::ECHO, "ECHO" }
};

static std::vector<std::pair<nl::network::FLAG, unsigned int>> types = {
	{ nl::network::FLAG::UP, IFF_UP },
	{ nl::network::FLAG::BROADCAST, IFF_BROADCAST },
	{ nl::network::FLAG::DEBUG, IFF_DEBUG },
	{ nl::network::FLAG::LOOPBACK, IFF_LOOPBACK },
	{ nl::network::FLAG::POINTOPOINT, IFF_POINTOPOINT },
	{ nl::network::FLAG::RUNNING, IFF_RUNNING },
	{ nl::network::FLAG::NOARP, IFF_NOARP },
	{ nl::network::FLAG::PROMISC, IFF_PROMISC },
	{ nl::network::FLAG::NOTRAILERS, IFF_NOTRAILERS },
	{ nl::network::FLAG::ALLMULTI, IFF_ALLMULTI },
	{ nl::network::FLAG::MASTER, IFF_MASTER },
	{ nl::network::FLAG::SLAVE, IFF_SLAVE },
	{ nl::network::FLAG::MULTICAST, IFF_MULTICAST },
	{ nl::network::FLAG::PORTSEL, IFF_PORTSEL },
	{ nl::network::FLAG::AUTOMEDIA, IFF_AUTOMEDIA },
	{ nl::network::FLAG::DYNAMIC, IFF_DYNAMIC },
	{ nl::network::FLAG::LOWER_UP, IFF_LOWER_UP },
	{ nl::network::FLAG::DORMANT, IFF_DORMANT },
	{ nl::network::FLAG::ECHO, IFF_ECHO }
};

std::vector<nl::network::INFO> nl::network::interfaces() {

	std::vector<nl::network::INFO> list;
	ifaddrs *ifaddr;

	if ( ::getifaddrs(&ifaddr) == -1 )
		throw std::runtime_error("failed to get interface addresses");

	for ( ifaddrs* ifa = ifaddr; ifa != nullptr; ifa = ifa -> ifa_next ) {

		nl::network::INFO* ifd = nullptr;
		std::string ifd_name(ifa -> ifa_name);
		if ( auto it = std::find_if(list.begin(), list.end(), [&ifd_name](const nl::network::INFO& i) { return i.ifd.to_string() == ifd_name; });
			it != list.end()) ifd = &(*it);
		else {
			list.push_back({ .ifd = ifd_name });
			ifd = &list.back();
		}

		sa_family_t family = ifa -> ifa_addr -> sa_family;

		if ( ifa -> ifa_flags != 0 ) {

			for ( const std::pair<nl::network::FLAG, unsigned int>& p : types )
				if ( ifa -> ifa_flags & p.second )
					ifd -> flags.emplace(p.first);
		}

		if ( family == AF_INET ) {

			char buf[INET_ADDRSTRLEN];

			if ( ifa -> ifa_addr != nullptr &&
				::inet_ntop(family, &((sockaddr_in*)(ifa -> ifa_addr)) -> sin_addr, buf, INET_ADDRSTRLEN) != nullptr )
				ifd -> ip_address = std::string(buf);

			if ( ifa -> ifa_netmask != nullptr &&
				::inet_ntop(family, &((sockaddr_in*)(ifa -> ifa_netmask)) -> sin_addr, buf, INET_ADDRSTRLEN) != nullptr )
				ifd -> prefix = nl::prefix(nl::ipaddress(std::string(buf)));

			if ( ifa -> ifa_flags & IFF_BROADCAST && ifa -> ifa_broadaddr != nullptr &&
				::inet_ntop(family, &((sockaddr_in*)(ifa -> ifa_broadaddr)) -> sin_addr, buf, INET_ADDRSTRLEN) != nullptr )
				ifd -> broadcast = std::string(buf);

			if ( ifa -> ifa_flags & IFF_POINTOPOINT && ifa -> ifa_dstaddr != nullptr &&
				::inet_ntop(family, &((sockaddr_in*)(ifa -> ifa_dstaddr)) -> sin_addr, buf, INET_ADDRSTRLEN) != nullptr )
				ifd -> dstaddr = std::string(buf);

		} else if ( family == AF_INET6 && ifa -> ifa_addr != nullptr ) {

			char buf[INET6_ADDRSTRLEN];

			if ( ::inet_ntop(family, &((sockaddr_in6*)(ifa -> ifa_addr)) -> sin6_addr, buf, INET6_ADDRSTRLEN) != nullptr )
				ifd -> ip6_address = std::string(buf);

		} else if ( family == AF_PACKET ) {

			if ( ifa -> ifa_data != nullptr ) {

				rtnl_link_stats* stats = static_cast<rtnl_link_stats*>(ifa -> ifa_data);
				ifd -> rx.packets = (unsigned long)stats -> rx_packets;
				ifd -> rx.bytes = (unsigned long)stats -> rx_bytes;
				ifd -> tx.packets = (unsigned long)stats -> tx_packets;
				ifd -> tx.bytes = (unsigned long)stats -> tx_bytes;
			}

		}
	}

	return list;
}

std::ostream& operator <<(std::ostream& os, const std::vector<nl::network::INFO>& vec) {

	std::string res;

	for ( const nl::network::INFO& ifd : vec ) {

		if ( !res.empty())
			res += "\n";

		res += ifd.ifd.to_string();

		if ( !ifd.flags.empty()) {

			res += "\n\tflags:";

			for ( const auto& flag : ifd.flags )
				res += " " + names[flag];
		}

		if ( !ifd.ip_address.empty()) {

			res += "\n\tIPv4: " + ifd.ip_address.to_string() + "/" + ifd.prefix.to_string() + " netmask: " + ifd.prefix.netmask().to_string();

			if ( !ifd.broadcast.empty())
				res += " broadcast: " + ifd.broadcast.to_string();
			else if ( ifd.dstaddr.empty())
				res += " broadcast: " + ifd.ip_address.broadcast(ifd.prefix).to_string();
			else
				res += " destination: " + ifd.dstaddr.to_string();
		}

		if ( !ifd.ip6_address.empty())
			res += "\n\tIPv6: " + ifd.ip6_address;

		res += "\n\trx: " + std::to_string(ifd.rx.bytes) + " bytes / " + std::to_string(ifd.rx.packets) + " packets\n"
			+ "\ttx: " + std::to_string(ifd.tx.bytes) + " bytes / " + std::to_string(ifd.tx.packets) + " packets\n";
	}

	os << res;
	return os;
}
