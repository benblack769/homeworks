
CXX=clang++-5.0   # Can switch to g++ if desired
CXXFLAGS=-g -std=c++17 -Wall -pedantic -Wextra -Werror
LDFLAGS=$(CXXFLAGS)
LIBS=

INC=vectree.hh tree.hh catch.hpp
SRC=test_tree.cc vectree.cc catch.cc ptrtree.cc
OBJ=$(SRC:.cc=.o)

all: test_tree

test_tree: $(OBJ)
	$(CXX) $(LDFLAGS) $(LIBS) -o $@ $(OBJ)

%.o.cc: %.cc %.hh
	$(CXX) $(CFLAGS) -c -o $@ $<

test: all
	./test_tree

clean:
	rm -f $(OBJ)
