[![License:MIT](https://img.shields.io/badge/License-MIT-blue?style=plastic)](LICENSE)
[![C++ CI build](../../actions/workflows/build.yml/badge.svg)](../../actions/workflows/build.yml)

### netlink_cpp
Small nettling library

### <sub>Description</sub>
Some netlink libraries for C/C++ already exist, but so far from those that I found,
they are quite huge, compilicated and possibly need to be linked against other libraries
and are too complicated or large to be inlined; this library is quite small. It does not
include as-is, everything, but you can do a lot more with it, what provided example shows,
example only uses "pre-defined" functions and you don't need to use functions at all, you
can create your own message structs. What this library is missing, is information retrieval;
you can send messages to netlink socket with this library, but support for answers is not
currently available through this library, so you cannot use this for the moment, for such
operations as retrieving link information etc.

Aim of this library is to create a simple and fast to use/write netlink operations; very
often when netlink socket is involved, code is very long and I wanted to simplify and
shorten this.

### <sub>Inspiration</sub>
I was quite inspired by [this](https://github.com/dmolik/netlink-examples/tree/master),
and I wanted to build a similar example with same idea. I also took unshare process
a bit further. Quite impressive and I am very grateful for work of dmolink for which
helped me a lot while building this library.

### <sub>Advanced usage</sub>
If you want to use features that are not provided by default, you should
check [iproute2](https://github.com/iproute2/iproute2) for guidance on how to
construct your message structs, pretty much everything available with ip command
besides information retrieval, due to missing answer parsing- can be done with
this library as well.

### <sub>Example</sub>
Sample code is provided, run as root.

It creates bridge br-lan if it does not exist, then it creates a veth pair,
after this, it builds a unshared private environment and uses veth pair
to add connectivity to that namespace and opens a shell for you to test
this connectivity. Finally when you exit the shell, it removes veth to
restore system to what it originally was (except that br-lan bridge is
not removed).
