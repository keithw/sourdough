AM_CXXFLAGS = $(PICKY_CXXFLAGS)
AM_CPPFLAGS = -I$(srcdir)/../src
LDADD = ../src/libsourdough.a -lpthread

bin_PROGRAMS = tcpclient tcpserver

tcpclient_SOURCES = tcpclient.cc

tcpserver_SOURCES = tcpserver.cc
