AM_CXXFLAGS = $(PICKY_CXXFLAGS)
AM_CPPFLAGS = -I$(srcdir)/../src
LDADD = ../src/libsourdough.a -lpthread

common_source = contest_message.hh contest_message.cc \
	controller.hh controller.cc

bin_PROGRAMS = sender receiver

sender_SOURCES = $(common_source) sender.cc

receiver_SOURCES = $(common_source) receiver.cc
