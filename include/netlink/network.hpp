#pragma once

#include <algorithm>
#include <string>
#include <cstring>
#include <vector>
#include <sstream>

#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mount.h>
#include <syscall.h>

#include "netlink.hpp"

#include <iostream>

#include <set>
#include <map>
#include <netdb.h>
#include <ifaddrs.h>
#include <linux/if_link.h>

#include <set>
#include <string>
#include <vector>
#include "netlink/types.hpp"

namespace nl::network {

	enum FLAG {
		UP, BROADCAST, DEBUG, LOOPBACK, POINTOPOINT, RUNNING, NOARP, PROMISC, NOTRAILERS,
		ALLMULTI, MASTER, SLAVE, MULTICAST, PORTSEL, AUTOMEDIA, DYNAMIC, LOWER_UP, DORMANT, ECHO
	};

	struct INFO {

		public:

			struct STATS {
				unsigned long packets = 0;
				unsigned long bytes = 0;
			};

			nl::interface ifd;
			nl::ipaddress ip_address;
			std::string ip6_address = "";
			nl::prefix prefix;
			nl::ipaddress broadcast;
			nl::ipaddress dstaddr;

			STATS rx;
			STATS tx;

			std::set<nl::network::FLAG> flags;
	};

	std::string to_string(const nl::network::FLAG& flag);
	std::vector<nl::network::INFO> interfaces();
}

std::ostream& operator <<(std::ostream& os, const std::vector<nl::network::INFO>& vec);
