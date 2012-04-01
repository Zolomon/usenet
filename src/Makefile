#
# Makefile to make the file libclientserver.a, containing
# connection.o and server.o
#

PREFIX		= ..
CXX		 	= g++
CXXFLAGS	= -ggdb -Wall -W -Werror -pedantic-errors
CXXFLAGS	+= -Wmissing-braces -Wparentheses -Wno-long-long
# The following option cannot be used since some of the socket
# macros give warnings on "old-style-cast"
#CXXFLAGS	+= -Wold-style-cast

# Define linker flags
LDFLAGS 	= -ggdb -L. -ldl -lpthread

# Libraries: -lclientserver is always necessary
LDLIBS 		= -lclientserver -lsqlite3

SRC	= $(wildcard *.cc)

.PHONY: all clean cleaner

all: libclientserver.a libsqlite3.a memserver client
	mkdir -p $(PREFIX)/bin
	mv usenetserver $(PREFIX)/bin/
	mv usenetclient $(PREFIX)/bin/

memserver: libclientserver.a libsqlite3.a UseNetServer.o MessageHandler.o DatabaseRAM.o Article.o NewsGroup.o server.o DatabaseDB.o
	$(CXX) $(CXXFLAGS)  UseNetServer.cc MessageHandler.cc DatabaseRAM.cc Article.cc NewsGroup.cc server.cc connection.cc DatabaseDB.cc -o usenetserver $(LDFLAGS) $(LDLIBS)

client: UseNetClient.o MessageHandler.o protocol.h connection.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(LDLIBS) -o usenetclient UseNetClient.cc MessageHandler.cc connection.cc

sqlite3.o: sqlite/sqlite3.c sqlite/sqlite3.h
	gcc -c -o sqlite3.o sqlite/sqlite3.c

# Create the library; ranlib is for Darwin and maybe other systems.
# Doesn't seem to do any damage on other systems.

libsqlite3.a: sqlite3.o
	ar rv libsqlite3.a \
	sqlite3.o
	ranlib libsqlite3.a

libclientserver.a: connection.o server.o
	ar rv libclientserver.a \
	connection.o server.o
	ranlib libclientserver.a

clean:
	$(RM) *.o usenetserver usenetclient usenet.sqlite

cleaner: clean
	$(RM) libclientserver.a libsqlite3.a
	$(RM) -r $(PREFIX)/bin

%.d: %.cc
	@set -e; rm -f $@; \
	 $(CXX) -MM $(CPPFLAGS) $< > $@.$$$$; \
	 sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	 rm -f $@.$$$$

include $(SRC:.cc=.d)
