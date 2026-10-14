CXX = mpicxx
CXXFLAGS = -std=c++23 -g3 -Wall -Wextra --pedantic -O3 -DNDEBUG

LIBS_DIR = ../../LIBS

tsp: tsp.cpp $(LIBS_DIR)/PRNG/random.cpp $(LIBS_DIR)/GA/PT.cpp $(LIBS_DIR)/GA/trip.cpp
	$(CXX) -o $@ $^ $(CXXFLAGS)

clean:
	rm -f *.o tsp
	rm -f ../OUTPUT/*.dat