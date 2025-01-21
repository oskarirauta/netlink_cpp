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

int main(int argc, char** argv) {

	nl::interface bridge("br-lan");
	nl::interface veth("host10");
	nl::interface peer("cntr10");
	nl::interface lo("lo");
	nl::prefix prefix = 16;
	nl::ipaddress veth_addr("10.0.12.1");
	std::pair<int,int> cnt = { 0, 0 };

	nl::socket nl_sock;
	nl_sock.open();

	try {
		std::cout << "\n - opening netlink socket connection: ";
		nl_sock.open();

		if ( nl_sock.is_open())
			std::cout << "success" << std::endl;
		else {
			std::cout << "failed" << std::endl;
			return 1;
		}

	} catch ( const std::runtime_error& e) {
		std::cout << "abort, " << e.what() << std::endl;
		return 1;
	}

	if ( !bridge.exists()) {

		try {
			std::cout << " - creating bridge " << bridge.to_string() << ":" << peer.to_string() << ", ";
			cnt = nl_sock.perform(nl::msg::add_interface(bridge, "bridge"));
			std::cout << "sent " << cnt.first << " bytes, received " << cnt.second << " bytes" << std::endl;
		} catch ( const std::runtime_error& e ) { std::cout << "failed: " << e.what() << std::endl; nl_sock.close(); return 1; }

	} else std::cout << " - ignoring creation of bridge " << bridge.to_string() << ", interface already exists" << std::endl;

	try {
		std::cout << " - creating veth pair " << veth.to_string() << ":" << peer.to_string() << ", ";
		cnt = nl_sock.perform(nl::msg::create_veth_pair(veth, peer));
		std::cout << "sent " << cnt.first << " bytes, received " << cnt.second << " bytes" << std::endl;
	} catch ( const std::runtime_error& e ) { std::cout << "failed: " << e.what() << std::endl; nl_sock.close(); return 1; }

	try {
		std::cout << " - setting " << peer.to_string() << " master to " << bridge.to_string() << ", ";
		cnt = nl_sock.perform(nl::msg::set_master(peer, bridge));
		std::cout << "sent " << cnt.first << " bytes, received " << cnt.second << " bytes" << std::endl;
	} catch ( const std::runtime_error& e) { std::cout << "failed: " << e.what() << std::endl; nl_sock.close(); return 1; }

	try {
		std::cout << " - bringing " << peer.to_string() << " up, ";
		cnt = nl_sock.perform(nl::msg::bring_up(peer));
		std::cout << "sent " << cnt.first << " bytes, received " << cnt.second << " bytes" << std::endl;
	} catch ( const std::runtime_error& e) { std::cout << "failed: " << e.what() << std::endl; nl_sock.close(); return 1; }


	pid_t f;
	int flags = 0 | CLONE_NEWPID | CLONE_NEWNS | CLONE_NEWNET | CLONE_NEWUTS | CLONE_NEWCGROUP | CLONE_NEWIPC;
	//int flags = CLONE_NEWNET;

	int to_child[2];
	int to_parent[2];
	::pipe(to_child);
	::pipe(to_parent);

	if ( f = ::syscall(__NR_clone, flags | SIGCHLD, 0, 0, 0, 0); f == 0 ) {


		::close(to_child[1]);
		::close(to_parent[0]);
		nl_sock.close();

		if ( ::mount(nullptr, "/", nullptr, MS_REC | MS_PRIVATE, nullptr) == -1 ) {
			std::cout << "mount failed: MS_PRIVATE" << std::endl;
			nl_sock.close();
			return 1;
		}

		if ( ::mount(nullptr, "/", "bind", MS_REMOUNT | MS_BIND, nullptr) != 0 ) {
			std::cout << "failed to remount root" << std::endl;
			nl_sock.close();
			return 1;
		}

		if ( ::chdir("/") != 0 ) {
			std::cout << "failed to chdir /, error: " << strerror(errno) << std::endl;
			nl_sock.close();
			return 1;
		}

		if ( ::umount2("/proc", MNT_DETACH) != 0 ) {
			std::cout << "unmount /proc failed, error: " << strerror(errno) << std::endl;
			nl_sock.close();
			return 1;
		}

		if ( ::mount("proc", "/proc", "proc", MS_NOATIME | MS_NODEV | MS_NOEXEC | MS_NOSUID, nullptr) != 0 ) {
			std::cout << "failed to mount proc" << std::endl;
			nl_sock.close();
			return 1;
		}

		//nl_sock.close();
		nl_sock.open();

		try {
			std::cout << " - bringing lo up, ";
			cnt = nl_sock.perform(nl::msg::bring_up(lo));
			std::cout << "sent " << cnt.first << " bytes, received " << cnt.second << " bytes" << std::endl;
		} catch ( const std::runtime_error& e) { std::cout << "failed: " << e.what() << std::endl; nl_sock.close(); return 1; }

		std::cout << " - child process is waiting for a signal from parent process" << std::endl;

		char c;
		do {
			if ( ::read(to_child[0], &c, 1))
				break;
		} while ( c != '\n' );

		std::cout << " - parent sent ready signal, child is proceeding" << std::endl;

		try {
			std::cout << " - adding " << veth_addr.to_string() << " as " << veth.to_string() << " address, ";
			cnt = nl_sock.perform(nl::msg::add_ip_address(veth, veth_addr, prefix));
			std::cout << "sent " << cnt.first << " bytes, received " << cnt.second << " bytes" << std::endl;
		} catch ( const std::runtime_error& e) { std::cout << "failed: " << e.what() << std::endl; nl_sock.close(); return 1; }

		try {
			std::cout << " - bringing " << veth.to_string() << " up, ";
			cnt = nl_sock.perform(nl::msg::bring_up(veth));
			std::cout << "sent " << cnt.first << " bytes, received " << cnt.second << " bytes" << std::endl;
		} catch ( const std::runtime_error& e) { std::cout << "failed: " << e.what() << std::endl; nl_sock.close(); return 1; }

		nl::interface inner("eth0");

		try {
			std::cout << " - renaming " << veth.to_string() << " to " << inner.to_string() << ", ";
			cnt = nl_sock.perform(nl::msg::rename_interface(veth, inner));
			std::cout << "sent " << cnt.first << " bytes, received " << cnt.second << " bytes" << std::endl;
		} catch ( const std::runtime_error& e) { std::cout << "failed: " << e.what() << std::endl; nl_sock.close(); return 1; }


		nl_sock.close();

		char *sh_args[2];
		sh_args[0] = strdup("/bin/sh");
		sh_args[1] = nullptr;

		execvp(sh_args[0], sh_args);
		free(sh_args[0]);
		exit(0);

	} else if ( f == -1 ) {

		std::cerr << "\nfork failed" << std::endl;
		::close(to_child[0]);
		::close(to_child[1]);
		::close(to_parent[0]);
		::close(to_parent[1]);

		nl_sock.close();
		return 1;
	}

	::close(to_parent[1]);
	::close(to_child[0]);

	std::cout << " - forked with syscall, pid of forked process: " << f << std::endl;

	nl::netns netns(f);
	try {
		std::cout << " - opening net namespace for pid " << f;
		netns.open();
		std::cout << std::endl;
	} catch ( const std::runtime_error& e ) { std::cout << " failed, error: " << e.what() << std::endl; nl_sock.close(); return 1; }

	try {
		std::cout << " - joining " << veth.to_string() << " to namespace " << (std::string)netns << ", ";
		cnt = nl_sock.perform(nl::msg::join_ns(veth, netns));
		std::cout << "sent " << cnt.first << " bytes, received " << cnt.second << " bytes" << std::endl;
	} catch ( const std::runtime_error& e) { std::cout << "failed: " << e.what() << std::endl; nl_sock.close(); return 1; }

	netns.close();

	std::cout << " - parent is ready, sending signal to child process" << std::endl;
	::write(to_child[1], "done\n", 5);

	int status;
	pid_t w;

	if ( w = ::waitpid(f, &status, 0); w == -1 ) {

		std::cerr << "\nwaitpid failed" << std::endl;
		nl_sock.close();
		return 1;
	}

	if ( WIFEXITED(status))
		std::cout << "\n - fork exited, status " << WEXITSTATUS(status) << std::endl;
	else if ( WIFSIGNALED(status))
		std::cout << "\n - fork killed by signal " << WTERMSIG(status) << std::endl;
	else if ( WIFSTOPPED(status))
		std::cout << "\n - fork stopped by signal " << WSTOPSIG(status) << std::endl;
	else if ( WIFCONTINUED(status))
		std::cout << "\n - continued" << std::endl;
	else std::cout << "\n";

	std::cout << " - proceeding on main program" << std::endl;

	try {
		std::cout << " - bringing " << peer.to_string() << " down, ";
		cnt = nl_sock.perform(nl::msg::bring_down(peer));
		std::cout << "sent " << cnt.first << " bytes, received " << cnt.second << " bytes" << std::endl;
	} catch ( const std::runtime_error& e) { std::cout << "failed: " << e.what() << std::endl; nl_sock.close(); return 1; }

	try {
		std::cout << " - deleting link " << peer.to_string() << ", ";
		cnt = nl_sock.perform(nl::msg::delete_link(peer));
		std::cout << "sent " << cnt.first << " bytes, received " << cnt.second << " bytes" << std::endl;
	} catch ( const std::runtime_error& e) { std::cout << "failed: " << e.what() << std::endl; nl_sock.close(); return 1; }

	nl_sock.close();

	std::cout << "\nend-of-program" << std::endl;

	return 0;
}
