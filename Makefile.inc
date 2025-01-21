NETLINK_DIR?=./netlink
INCLUDES += -I$(NETLINK_DIR)/include

NETLINK_OBJS:= \
	objs/nl_ipaddress.o \
	objs/nl_types.o \
	objs/nl_prefix.o \
	objs/nl_interface.o \
	objs/nl_netns.o \
	objs/nl_attr.o \
	objs/nl_property.o \
	objs/nl_msg.o \
	objs/nl_socket.o \
	objs/main.o

objs/nl_ipaddress.o: $(NETLINK_DIR)/src/ipaddress.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

objs/nl_prefix.o: $(NETLINK_DIR)/src/prefix.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

objs/nl_interface.o: $(NETLINK_DIR)/src/interface.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

objs/nl_attr.o: $(NETLINK_DIR)/src/attr.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

objs/nl_types.o: $(NETLINK_DIR)/src/types.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

objs/nl_netns.o: $(NETLINK_DIR)/src/netns.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

objs/nl_property.o: $(NETLINK_DIR)/src/property.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

objs/nl_msg.o: $(NETLINK_DIR)/src/msg.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

objs/nl_socket.o: $(NETLINK_DIR)/src/socket.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;
