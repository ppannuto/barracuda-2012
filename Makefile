CC = $(CXX)
CPPFLAGS += -Wall -Wextra
LDFLAGS +=
LIBS += $(shell xmlrpc-c-config c++2 abyss-server --libs)

all: main

main: main.o xmlrpc_methods.o game.o
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -rf *.o main

.PHONY: clean all
