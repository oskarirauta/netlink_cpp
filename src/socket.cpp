#include <algorithm>
#include <string>
#include <cstring>
#include <vector>
#include <sstream>
#include <stdexcept>

#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <unistd.h>
#include <fcntl.h>

#include "netlink/socket.hpp"

static const size_t hdrsize = 4096;

static rtattr* NLMSG_TAIL(nlmsghdr* nmsg) {

	return (rtattr*)
		((unsigned char*)nmsg + NLMSG_ALIGN(nmsg -> nlmsg_len));
}

static bool fd_is_open(int fd) {

	return ::fcntl(fd, F_GETFD) != -1 || errno != EBADF;
}

void nl::socket::open() {

	if ( this -> fd > -1 )
		return;

	if ( this -> fd = ::socket(AF_NETLINK, SOCK_RAW | SOCK_CLOEXEC, NETLINK_ROUTE); this -> fd < 0 ) {
		this -> fd = -1;
		throw std::runtime_error("failed to open socket, " + std::string(::strerror(errno)));
	}

	int recv_buf = 32768;
	int send_buf = 32768;
	int one = 1;

	if ( ::setsockopt(this -> fd, SOL_SOCKET, SO_SNDBUF, &send_buf, sizeof(send_buf)) < 0 ) {

		this -> close();
		throw std::runtime_error("failed to set socket send buffer, " + std::string(::strerror(errno)));
	}

	if ( ::setsockopt(this -> fd, SOL_SOCKET, SO_RCVBUF, &recv_buf, sizeof(recv_buf)) < 0 ) {

		this -> close();
		throw std::runtime_error("failed to set socket receive buffer, " + std::string(::strerror(errno)));
	}

	::setsockopt(this -> fd, SOL_NETLINK, NETLINK_EXT_ACK, &one, sizeof(one));

	sockaddr_nl sa = { .nl_family = AF_NETLINK, .nl_groups = 0 };

	if ( ::bind(this -> fd, (sockaddr*)&sa, sizeof(sa)) < 0 ) {

		this -> close();
		throw std::runtime_error("failed to bind to socket, " + std::string(::strerror(errno)));
	}
}

void nl::socket::close() {

	if ( this -> fd < 0 )
		return;
	else if ( fd_is_open(this -> fd))
		::close(this -> fd);

	this -> fd = -1;
}

bool nl::socket::is_open() {

	if ( this -> fd >= 0 && !fd_is_open(this -> fd))
		this -> fd = -1;

	return this -> fd >= 0;

}

bool nl::socket::is_open() const {

	return this -> fd >= 0 && fd_is_open(this -> fd);
}

static void copy_rtmsg(rtmsg *msg, const rtmsg& src) {

	msg -> rtm_family = src.rtm_family;
	msg -> rtm_dst_len = src.rtm_dst_len;
	msg -> rtm_src_len = src.rtm_src_len;
	msg -> rtm_tos = src.rtm_tos;
	msg -> rtm_table = src.rtm_table;
	msg -> rtm_protocol = src.rtm_protocol;
	msg -> rtm_scope = src.rtm_scope;
	msg -> rtm_type = src.rtm_type;
	msg -> rtm_flags = src.rtm_flags;
}

static void copy_ifinfomsg(ifinfomsg *msg, const ifinfomsg& src) {

	msg -> ifi_family = src.ifi_family;
	msg -> __ifi_pad = src.__ifi_pad;
	msg -> ifi_type = src.ifi_type;
	msg -> ifi_index = src.ifi_index;
	msg -> ifi_flags = src.ifi_flags;
	msg -> ifi_change = src.ifi_change;
}

static void copy_prefixmsg(prefixmsg *msg, const prefixmsg& src) {

	msg -> prefix_family = src.prefix_family;
	msg -> prefix_pad1 = src.prefix_pad1;
	msg -> prefix_pad2 = src.prefix_pad2;
	msg -> prefix_ifindex = src.prefix_ifindex;
	msg -> prefix_type = src.prefix_type;
	msg -> prefix_len = src.prefix_len;
	msg -> prefix_flags = src.prefix_flags;
	msg -> prefix_pad3 = src.prefix_pad3;
}

static void copy_ifaddrmsg(ifaddrmsg *msg, const ifaddrmsg& src) {

	msg -> ifa_family = src.ifa_family;
	msg -> ifa_prefixlen = src.ifa_prefixlen;
	msg -> ifa_flags = src.ifa_flags;
	msg -> ifa_scope = src.ifa_scope;
	msg -> ifa_index = src.ifa_index;
}

static void add_data(nlmsghdr* msg, int type, const nl::attr& attr) {

	rtattr* rta;
	size_t sz = attr.size();
	size_t rta_len = RTA_LENGTH(sz);
	rta = NLMSG_TAIL(msg);
	rta -> rta_type = type;
	rta -> rta_len = rta_len;
	if ( std::get<void*>(attr) != nullptr )
		std::memcpy(RTA_DATA(rta), std::get<void*>(attr), sz);
	msg -> nlmsg_len = NLMSG_ALIGN(msg -> nlmsg_len) + RTA_ALIGN(rta_len);
}

static void add_string(nlmsghdr* msg, int type, const std::string& s) {

	rtattr* rta;
	size_t sz = s.size() + 1;
	size_t rta_len = RTA_LENGTH(sz);
	rta = NLMSG_TAIL(msg);
	rta -> rta_type = type;
	rta -> rta_len = rta_len;
	std::memcpy(RTA_DATA(rta), s.c_str(), sz);
	msg -> nlmsg_len = NLMSG_ALIGN(msg -> nlmsg_len) + RTA_ALIGN(rta_len);
}

static void add_number(nlmsghdr* msg, int type, const nl::attr& attr) {

        rtattr* rta;
        size_t sz = attr.size();
        size_t rta_len = RTA_LENGTH(sz);
        rta = NLMSG_TAIL(msg);
        rta -> rta_type = type;
        rta -> rta_len = rta_len;

	void* data = new void*[sz];
	if ( attr.is_int()) {
		int n = attr.to_int();
		std::memcpy(data, &n, attr.size());
	} else if ( attr.is_u8()) {
		__u8 n = attr.to_u8();
		std::memcpy(data, &n, attr.size());
	} else if ( attr.is_u16()) {
		__u16 n = attr.to_u16();
		std::memcpy(data, &n, attr.size());
	} else if ( attr.is_u64()) {
		__u64 n = attr.to_u64();
		std::memcpy(data, &n, attr.size());
	}

	std::memcpy(RTA_DATA(rta), data, sz);
        msg -> nlmsg_len = NLMSG_ALIGN(msg -> nlmsg_len) + RTA_ALIGN(rta_len);
}

static void add_attr(nlmsghdr* msg, int type) {

	rtattr* rta;
	size_t sz = 0;
	size_t rta_len = RTA_LENGTH(sz);
	rta = NLMSG_TAIL(msg);
	rta -> rta_type = type;
	rta -> rta_len = rta_len;
	msg -> nlmsg_len = NLMSG_ALIGN(msg -> nlmsg_len) + RTA_ALIGN(rta_len);
}

static void add_ipaddress(nlmsghdr* msg, int type, const nl::attr& attr) {

	rtattr* rta;
	size_t sz = sizeof(in_addr);
	size_t rta_len = RTA_LENGTH(sz);
	rta = NLMSG_TAIL(msg);
	rta -> rta_type = type;
	rta -> rta_len = rta_len;

	in_addr addr = attr.to_ipaddress().to_in_addr();
	std::memcpy(RTA_DATA(rta), &addr, sz);
	msg -> nlmsg_len = NLMSG_ALIGN(msg -> nlmsg_len) + RTA_ALIGN(rta_len);
}

static void add_interface(nlmsghdr* msg, int type, const nl::attr& attr) {

	add_string(msg, type, (std::string)attr.to_interface());
}

static void add_prefix(nlmsghdr* msg, int type, const nl::attr& attr) {

	int value = (int)attr.to_prefix();
	nl::attr attr2 = (__u8)value;
	add_number(msg, type, attr2);
}

static void add_netns(nlmsghdr* msg, int type, const nl::attr& attr) {

	nl::netns ns = attr.to_netns();

	if ( !ns.is_open()) {

		try {
			ns.open();
		} catch ( const std::runtime_error& e ) {
			throw e;
		}

		if ( !ns.is_open())
			throw std::runtime_error("failed to open namespace file " + (std::string)ns);
	}

	int value = ns.fd();

	if ( value < 0 )
		throw std::runtime_error("failed to open namespace file " + (std::string)ns);

	nl::attr attr2 = (int)value;
	add_number(msg, type, attr2);
}

int nl::socket::send(const nl::msg& msg) const {

	size_t sz;
	nlmsghdr* hdr = new nlmsghdr[hdrsize];
	std::memset(hdr, 0, hdrsize);

	hdr -> nlmsg_type = (__u16)msg.type;
	hdr -> nlmsg_flags = (__u16)msg.flags;
	hdr -> nlmsg_seq = ::time(nullptr);
	hdr -> nlmsg_pid = 0;

	if ( std::holds_alternative<rtmsg>(msg.data)) {

		sz = sizeof(rtmsg);
		hdr -> nlmsg_len = NLMSG_LENGTH(sz);
		rtmsg* data = (rtmsg*)NLMSG_DATA(hdr);
		copy_rtmsg(data, std::get<rtmsg>(msg.data));

	} else if ( std::holds_alternative<ifinfomsg>(msg.data)) {

		sz = sizeof(ifinfomsg);
		hdr -> nlmsg_len = NLMSG_LENGTH(sz);
		ifinfomsg* data = (ifinfomsg*)NLMSG_DATA(hdr);
		copy_ifinfomsg(data, std::get<ifinfomsg>(msg.data));

	} else if ( std::holds_alternative<prefixmsg>(msg.data)) {

		sz = sizeof(prefixmsg);
		hdr -> nlmsg_len = NLMSG_LENGTH(sz);
		prefixmsg* data = (prefixmsg*)NLMSG_DATA(hdr);
		copy_prefixmsg(data, std::get<prefixmsg>(msg.data));

	} else if ( std::holds_alternative<ifaddrmsg>(msg.data)) {

		sz = sizeof(ifaddrmsg);
		hdr -> nlmsg_len = NLMSG_LENGTH(sz);
		ifaddrmsg* data = (ifaddrmsg*)NLMSG_DATA(hdr);
		copy_ifaddrmsg(data, std::get<ifaddrmsg>(msg.data));
	}

	for ( const nl::property& prop : msg.properties ) {

		if ( prop.second.is_string())
			add_string(hdr, prop.first, prop.second.to_string());
		else if ( prop.second.is_number())
			add_number(hdr, prop.first, prop.second);
		else if ( prop.second.is_data())
			add_data(hdr, prop.first, prop.second);
		else if ( prop.second.is_null())
			add_attr(hdr, prop.first);
		else if ( prop.second.is_ipaddress())
			add_ipaddress(hdr, prop.first, prop.second);
		else if ( prop.second.is_interface())
			add_interface(hdr, prop.first, prop.second);
		else if ( prop.second.is_prefix())
			add_prefix(hdr, prop.first, prop.second);
		else if ( prop.second.is_netns())
			add_netns(hdr, prop.first, prop.second);
		else continue; // unsupported attribute
	}

	std::vector<rtattr*> nests;

	for ( const std::pair<int, std::vector<nl::property>>& nest : msg.nested ) {

		nests.push_back(NLMSG_TAIL(hdr));
		add_attr(hdr, nest.first);

		for ( const nl::property& prop : nest.second ) {

			if ( prop.first == -1 && prop.second.is_null() && prop.second.size() == 0 )
				continue;
			else if ( prop.first == -1 && prop.second.is_null() && prop.second.size() > 0 ) {
				hdr -> nlmsg_len += prop.second.size();
			} else if ( prop.second.is_string())
				add_string(hdr, prop.first, prop.second.to_string());
			else if ( prop.second.is_number())
				add_number(hdr, prop.first, prop.second);
			else if ( prop.second.is_null())
				add_attr(hdr, prop.first);
			else if ( prop.second.is_ipaddress())
				add_ipaddress(hdr, prop.first, prop.second);
			else if ( prop.second.is_interface())
				add_interface(hdr, prop.first, prop.second);
			else if ( prop.second.is_prefix())
				add_prefix(hdr, prop.first, prop.second);
			else if ( prop.second.is_netns())
				add_netns(hdr, prop.first, prop.second);
			else continue; // unsupported attribute
		}
	}

	if ( !nests.empty()) {

		//hdr -> nlmsg_len += sz;

		for ( size_t i = nests.size(); i > 0; i-- )
			nests[i - 1] -> rta_len = (unsigned char *)NLMSG_TAIL(hdr) - (unsigned char *)nests[i - 1];
	}

	sockaddr_nl *sa = new sockaddr_nl;
	std::memset(sa, 0, sizeof(sockaddr_nl));
	sa -> nl_family = AF_NETLINK;

	iovec iov = { hdr, hdr -> nlmsg_len };
	msghdr msgdata = {
		.msg_name = sa,
		.msg_namelen = sizeof(*sa),
		.msg_iov = &iov,
		.msg_iovlen = 1,
		.msg_control = nullptr,
		.msg_controllen = 0,
		.msg_flags = 0
	};

	size_t cnt = ::sendmsg(this -> fd, &msgdata, 0 );

	delete sa;
	delete []hdr;

	if ( cnt < 0 )
		throw std::runtime_error("failed to send message to socket, " + std::string(::strerror(errno)));

	return cnt;
}

int nl::socket::recv() const {

	int cnt;
	sockaddr_nl *sa = new sockaddr_nl;
	std::memset(sa, 0, sizeof(sockaddr_nl));
	sa -> nl_family = AF_NETLINK;

	char buf[hdrsize];
	iovec  iov = { buf, hdrsize };
	msghdr msgdata = {
		.msg_name = sa,
		.msg_namelen = sizeof(*sa),
		.msg_iov = &iov,
		.msg_iovlen = 1,
		.msg_control = nullptr,
		.msg_controllen = 0,
		.msg_flags = 0
	};

	do {
		cnt = ::recvmsg(this -> fd, &msgdata, 0);
	} while ( cnt < 0 && ( errno == EINTR || errno == EAGAIN ));

	delete sa;

	if ( cnt < 0 )
		throw std::runtime_error("socket recv error " + std::to_string(errno) + ", " + std::string(::strerror(errno)));
	else if ( cnt == 0 )
		throw std::runtime_error("socket recv EOF, " + std::string(::strerror(ENODATA)));

	nlmsghdr *ret = (nlmsghdr*)buf;

	if ( ret -> nlmsg_type == NLMSG_ERROR ) {

		nlmsgerr *err = (nlmsgerr*)NLMSG_DATA(ret);

		if ( err -> error != 0 ) {

			std::string errmsg(::strerror(-(err -> error)));
			throw std::runtime_error(errmsg);
		}
	}

	return cnt;
}

std::pair<int, int> nl::socket::perform(const nl::msg& msg) const {

	std::pair<int, int> cnt = { 0, 0 };

	try {
		cnt.first = this -> send(msg);
	} catch ( const std::runtime_error& e ) {
		throw e;
	}

	try {
		cnt.second = this -> recv();
	} catch ( const std::runtime_error& e ) {
		throw e;
	}

	return cnt;
}

nl::socket::socket() { }

nl::socket::~socket() {

	this -> close();
}
