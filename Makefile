all: world

CXX?=g++
CXXFLAGS?=--std=c++17 -Wall -fPIC -g

EXAMPLE:= \
	objs/main.o

IFCONF:= \
	objs/ifconf.o

NETLINK_DIR:=.
include ./Makefile.inc

world: example ifconf

$(shell mkdir -p objs)

objs/main.o: main.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

objs/ifconf.o: ifconf.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

example: $(NETLINK_OBJS) $(EXAMPLE)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@;

ifconf: $(NETLINK_OBJS) $(IFCONF)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@;

.PHONY: clean
clean:
	@rm -rf objs example ifconf
