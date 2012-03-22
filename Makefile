#
# Makefile to make the file libclientserver.a, containing
# connection.o and server.o
#

CXX		 	= g++
CXXFLAGS	= -ggdb -Wall -W -Werror -pedantic-errors
CXXFLAGS	+= -Wmissing-braces -Wparentheses 
# The following option cannot be used since some of the socket
# macros give warnings on "old-style-cast"
#CXXFLAGS	+= -Wold-style-cast

# Define linker flags
LDFLAGS 	= -ggdb -L.

# Libraries: -lclientserver is always necessary
LDLIBS 		= -lclientserver

SRC	= $(wildcard *.cc)

# Payloads
PROGS		= usenetserver 

.PHONY: all clean cleaner

all: libclientserver.a memserver

memserver: libclientserver.a UseNetServer.o MessageHandler.o

# Create the library; ranlib is for Darwin and maybe other systems.
# Doesn't seem to do any damage on other systems.

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
