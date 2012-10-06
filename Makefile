CC = $(CXX)
CPPFLAGS += -Wall -Wextra
CPPFLAGS += -O0
CPPFLAGS += -g
LDFLAGS +=
LIBS += $(shell xmlrpc-c-config c++2 abyss-server --libs)

all: main

main: main.o xmlrpc_methods.o game.o strategy.o
	$(CXX) $(LDFLAGS) $(CPPFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -rf *.o main

.PHONY: clean all
