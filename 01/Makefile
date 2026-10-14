CXX = g++
CXXFLAGS = -std=c++23 -g3 -Wall -Wextra --pedantic

LIB_SRCS = ../LIBS/PRNG/random.cpp ../LIBS/DB/DataBlocking.cpp
LIB_HDRS = ../LIBS/PRNG/random.h ../LIBS/DB/DataBlocking.h

all: 01.1 01.2 01.3

01.1: 01.1.cpp $(LIB_SRCS) $(LIB_HDRS)
	$(CXX) -o $@ 01.1.cpp $(LIB_SRCS) $(CXXFLAGS)

01.2: 01.2.cpp $(LIB_SRCS) $(LIB_HDRS)
	$(CXX) -o $@ 01.2.cpp $(LIB_SRCS) $(CXXFLAGS)

01.3: 01.3.cpp $(LIB_SRCS) $(LIB_HDRS)
	$(CXX) -o $@ 01.3.cpp $(LIB_SRCS) $(CXXFLAGS)

clean:
	rm -f *.o 01.1 01.2 01.3 ./OUTPUT/*.dat


	