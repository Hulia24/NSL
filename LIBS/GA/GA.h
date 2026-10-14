#pragma once

#include <map>
#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include <random>

#include "../PRNG/random.h"
#include "trip.h"

class GA {

    private:

    Random& rnd;

    public:

    int _n_cities;
    int _n_in_population;
    multimap <double, trip> population;

    GA(int n_cities, int n_population, bool circle_trip, Random& r);
    GA(int n_cities, int n_population, std::string file_path, Random& r);
    ~GA(){};

    void write_cities_config( vector<double> x, vector<double> y, std::string file_path);
    void print_population(std::string file_path);
    vector<double> save_loss();
    void write_loss(int n_generation, std::string file_path);
    void write_mean_loss(int n_generation, std::string file_path);
    void save_trip(int i, int n_gen, std::string file_path);

    //selection algorithms:
    trip select_individual_probability(); //select a trip based on it's loss function
    trip select_individual_power(double p);

    //crossover:
    void crossover(trip &t1, trip &t2);


};
