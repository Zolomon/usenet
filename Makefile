#
# Makefile to make the file libclientserver.a, containing
# connection.o and server.o
#

CXX		 	= g++
CXXFLAGS	= -ggdb -Wall -W -Werror -pedantic-errors
CXXFLAGS	+= -Wmissing-braces -Wparentheses -Wno-long-long
# The following option cannot be used since some of the socket
# macros give warnings on "old-style-cast"
#CXXFLAGS	+= -Wold-style-cast

# Define linker flags
LDFLAGS 	= -ggdb -L.

# Libraries: -lclientserver is always necessary
LDLIBS 		= -lclientserver -lsqlite3

SRC	= $(wildcard *.cc)

# Payloads
PROGS		= usenetserver 

.PHONY: all clean cleaner

all: libclientserver.a memserver

memserver: libclientserver.a libsqlite3.a UseNetServer.o MessageHandler.o DatabaseRAM.o Article.o NewsGroup.o server.o DatabaseDB.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(LDLIBS) UseNetServer.cc MessageHandler.cc DatabaseRAM.cc Article.cc NewsGroup.cc server.cc connection.cc DatabaseDB.cc

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
	$(RM) *.o

cleaner: clean
	$(RM) libclientserver.a

%.d: %.cc
	@set -e; rm -f $@; \
	 $(CXX) -MM $(CPPFLAGS) $< > $@.$$$$; \
	 sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	 rm -f $@.$$$$

include $(SRC:.cc=.d)
