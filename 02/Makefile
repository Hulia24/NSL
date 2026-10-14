CXX = g++
CXXFLAGS = -std=c++23 -g3 -Wall -Wextra --pedantic

LIB_SRCS = ../LIBS/PRNG/random.cpp ../LIBS/DB/DataBlocking.cpp
LIB_HDRS = ../LIBS/PRNG/random.h ../LIBS/DB/DataBlocking.h

all: 02.1 02.2

02.1: 02.1.cpp $(LIB_SRCS) $(LIB_HDRS)
	$(CXX) -o $@ 02.1.cpp $(LIB_SRCS) $(CXXFLAGS)

02.2: 02.2.cpp $(LIB_SRCS) $(LIB_HDRS)
	$(CXX) -o $@ 02.2.cpp $(LIB_SRCS) $(CXXFLAGS)

clean:
	rm -f *.o 02.1 02.2 *.dat

