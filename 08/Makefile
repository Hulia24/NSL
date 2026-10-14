CXX      = g++
CXXFLAGS = -std=c++23 -O3 -Wall -Wextra --pedantic

LIBS_DIR = ../LIBS
COMMON   = $(LIBS_DIR)/DB/DataBlocking.cpp $(LIBS_DIR)/PRNG/random.cpp
HDRS     = $(LIBS_DIR)/SA/quant.h $(LIBS_DIR)/SA/Metropolis.h $(LIBS_DIR)/SA/SA.h \
           $(LIBS_DIR)/DB/DataBlocking.h $(LIBS_DIR)/PRNG/random.h

all: 8.1 8.2

8.1: 8.1.cpp $(COMMON) $(HDRS) Makefile
	$(CXX) -o $@ 8.1.cpp $(COMMON) $(CXXFLAGS)

8.2: 8.2.cpp $(COMMON) $(HDRS) Makefile
	$(CXX) -o $@ 8.2.cpp $(COMMON) $(CXXFLAGS)

clean:
	rm -f 8.1 8.2 *.o OUTPUT/*.dat