
# COMPILER FLAGS
CC = clang++
CFLAGS = -Wall -pedantic --std=c++11
DEBUG = 0

ifeq ($(DEBUG),1)
	OPTIMIZEFLAG = -g
else
	OPTIMIZEFLAG = -O3
endif

# OBJECTS
COMMON_OBJECTS = clientserver.cpp stats.cpp
CLIENT_OBJECTS = dummyclient.cpp
CLIENT_OUTPUT = dummyclient

# DEPENDENCIES

all:	client
client: $(CLIENT_OBJECTS) $(COMMON_OBJECTS)
	$(CC) $(CFLAGS) $(OPTIMIZEFLAG) -o $(CLIENT_OUTPUT) $(CLIENT_OBJECTS) $(COMMON_OBJECTS)

clean:
	rm -rf $(SERVER_OUTPUT) $(CLIENT_OUTPUT)
