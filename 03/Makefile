CXX = g++
CXXFLAGS = -std=c++23 -g3 -Wall -Wextra --pedantic

LIB_SRCS = ../LIBS/PRNG/random.cpp ../LIBS/DB/DataBlocking.cpp
LIB_HDRS = ../LIBS/PRNG/random.h ../LIBS/DB/DataBlocking.h

03.1: 03.1.cpp $(LIB_SRCS) $(LIB_HDRS)
	$(CXX) -o $@ 03.1.cpp $(LIB_SRCS) $(CXXFLAGS)

clean:
	rm -f *.o 03.1 ./OUTPUT/*.dat


	