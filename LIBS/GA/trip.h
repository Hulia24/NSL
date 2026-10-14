#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <cmath>
#include <random>

#include "../PRNG/random.h"

using namespace std;

struct city{

    int id;
    double x, y;

    city() : id(0), x(0.), y(0.) {}
    city(int id, double x, double y) : id(id), x(x), y(y) {}

};

class trip{
    public:

    int _n_cities;
    vector<city> cities;


    trip(): _n_cities(0) {} //default costructor
    trip(int n_cities, vector<city> cities): _n_cities(n_cities), cities(cities) {}
    trip( int n_cities, string file_path, Random& rand);

    int get_n_cities() {return _n_cities;}
    double distance_L1();
    vector<int> get_path();
    void check(); //checks if all the cities are unique 

    // mutations:
    void random_swap_mutation(Random &rnd);
    void shift_mutation(Random &rnd, int block_length, int n_shift); 
    void inversion_mutation(Random &rnd, int block_length);
    void block_permutation_mutation(Random &rnd, int block_length);


};