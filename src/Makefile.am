AM_CXXFLAGS = $(PICKY_CXXFLAGS)

sourdough_sources = util.hh \
	file_descriptor.hh file_descriptor.cc \
	address.hh address.cc \
	socket.hh socket.cc \
	poller.hh poller.cc

bin_PROGRAMS = tcpserver tcpclient

tcpserver_SOURCES = $(sourdough_sources) tcpserver.cc
tcpserver_LDADD = -lpthread

tcpclient_SOURCES = $(sourdough_sources) tcpclient.cc
tcpclient_LDADD = -lpthread
