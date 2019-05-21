CLIENT          = ipk-dhcpstarve
CLIENT_SOURCES  = ipk-dhcpstarve.cpp

DEFINES         = 

CFLAGS         = -std=c++11 

LIBRARIES       = -static-libstdc++

CC              = g++
CLIENT_OBJECTS  = $(CLIENT_SOURCES:.cpp=.o)
INCLUDES        = #-I.
LIBDIRS         = 
LDFLAGS         = $(LIBDIRS) $(LIBRARIES)

###########################################

.SUFFIXES: .cpp .o

.cpp.o:
		$(CC) $(CFLAGS) -c $<

###########################################

all:		$(CLIENT)

rebuild:	clean all

$(CLIENT):	$(CLIENT_OBJECTS)
		$(CC) $(CLIENT_OBJECTS) $(LDFLAGS) -o $@

###########################################

clean:
	rm -fr core* *~ $(CLIENT_OBJECTS)  $(CLIENT) .make.state .sb
