CC   		:= g++ -std=c++1z
CXXFLAGS	:= -Wpedantic -Wall -Wextra -Wuninitialized -O3 -Werror
BIN			:= cbqueue

all: $(BIN)

$(BIN): main.cpp cbqueue.h cbqueue.hpp
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -f $(BIN)
