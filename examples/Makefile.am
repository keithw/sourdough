AM_CPPFLAGS = $(CXX11_FLAGS) -I$(srcdir)/../src
AM_CXXFLAGS = $(PICKY_CXXFLAGS)
LDADD = ../src/libsourdough.a -lpthread

bin_PROGRAMS = tcpclient tcpserver

tcpclient_SOURCES = tcpclient.cc

tcpserver_SOURCES = tcpserver.cc
