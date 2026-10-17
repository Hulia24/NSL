#pragma once

#include <vector>
#include <string>
#include <mpi.h>

#include "trip.h"
#include "../PRNG/random.h"

class PT {

    private:
    
    //Parameters for simulated annealing:
    trip _current_trip;
    trip _best_trip;
    double _current_loss;
    double _best_loss;
    double _temp;
    Random _rnd;

    //Parameters for parallel tempering:
    int _rank;
    int _size;
    int _proposed_swaps; //coubted only by the lower rank of each pair
    int _accepted_swaps;

    public:

    PT( int n_cities, const std::string& namefile, Random& rnd );

    void set_temperature( double T) { _temp = T; }
    void set_trip(const trip& t) { _current_trip = t; }

    double get_temperature() { return _temp; }
    const trip& get_best_trip() { return _best_trip; }
    double get_current_loss() { return _current_loss; }
    double get_best_loss() { return _best_trip.distance_L1(); }
    int get_accepted_swaps() { return _accepted_swaps; }
    int get_proposed_swaps() { return _proposed_swaps; }

    bool Metro_step(); //One Metropolis step at current temperature
    bool PT_swap(int step, int swap_freq); //Parallel tempering swap called by all ranks at every step

};