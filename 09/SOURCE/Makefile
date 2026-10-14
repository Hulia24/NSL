CXXFLAGS = -std=c++23 -g3 -Wall -Wextra --pedantic

LIBS_DIR = ../../LIBS

genetic: genetic.cpp $(LIBS_DIR)/PRNG/random.cpp $(LIBS_DIR)/GA/GA.cpp $(LIBS_DIR)/GA/trip.cpp 
	g++ -o $@ $^ $(CXXFLAGS)

clean:
	rm -f *.o genetic
	rm -f ../OUTPUT/*.dat

