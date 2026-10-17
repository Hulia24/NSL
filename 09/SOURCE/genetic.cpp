#include <map>
#include <vector>
#include <fstream>
#include <iostream>
#include <chrono>    //to confront time with PT

#include "../../LIBS/GA/GA.h"
#include "../../LIBS/GA/trip.h"
#include "../../LIBS/PRNG/random.h"


int main(int argc, char *argv[]){

    if(argc != 2) {
        cerr << "Usage: " << argv[0] << " circle, square or italy" << endl;          
        return 1;
    }

    string arg = argv[1];

    if(arg != "circle" && arg != "square" && arg != "italy") {                       
        cerr << "Error: invalid argument. Use 'circle', 'square' or 'italy'" << endl;
        return 1;
    }

    bool circle_path = arg == "circle";
    bool italy = arg == "italy";                                               

    int n_cities     = italy ? 110 : 34;                                            
    int n_population = 300;
    int n_generation = italy ? 20000 : (circle_path ? 200 : 500);                    

    Random rnd("../../LIBS/PRNG/Primes", "../../LIBS/PRNG/seed.in");

    const std::string cities_file = "../INPUT/cap_prov_ita.dat";
    GA tsp = italy ? GA(n_cities, n_population, cities_file, rnd) : GA(n_cities, n_population, circle_path, rnd);

    //output files:
    auto init_file = [](const std::string& filename, const std::string& header) {
        std::ofstream out(filename, std::ios::trunc);
        if (!out) {
            std::cerr << "Error opening file: " << filename << std::endl;
            std::exit(EXIT_FAILURE);
        }
        out << header << "\n";
    };

    init_file("../OUTPUT/best_loss_" + arg + ".dat", "Generation Loss");
    init_file("../OUTPUT/mean_loss_" + arg + ".dat", "Generation Mean Loss");
    init_file("../OUTPUT/best_trip_" + arg + ".dat", "Generation Loss Trip");

    // generation 0: initial population
    tsp.save_trip(0, 0, "../OUTPUT/best_trip_" + arg + ".dat");
    tsp.write_loss(0,   "../OUTPUT/best_loss_" + arg + ".dat");
    tsp.write_mean_loss(0, "../OUTPUT/mean_loss_" + arg + ".dat");

    //mutations protocoll:
    auto short_len = [&](int lo, int hi) {
        double r = rnd.Rannyu();
        return lo + int((hi - lo + 1) * r * r); //to favor short mutations i used r^2
    };

    auto mutate = [&](trip& t) {
        if (rnd.Rannyu() < 0.10) t.random_swap_mutation(rnd);
        if (rnd.Rannyu() < 0.10) t.shift_mutation(rnd, short_len(1, 8), short_len(1, 6));
        if (rnd.Rannyu() < 0.10) t.block_permutation_mutation(rnd, short_len(1, n_cities/2 - 1));
        if (rnd.Rannyu() < 0.10) t.inversion_mutation(rnd, short_len(2, n_cities - 1));
    };

    auto t0 = chrono::steady_clock::now(); 

    for (int i = 0; i < n_generation; i++) {

        multimap<double, trip> new_population;

        //elitism for keeping the best trip overall:
        auto best_individual_it = tsp.population.begin();
        new_population.insert({best_individual_it->first, best_individual_it->second});

        for (int j = 0; j < n_population/2; j++) {

            // Two parents from the population
            trip t1 = tsp.select_individual_power(2.0);
            trip t2 = tsp.select_individual_power(2.0);

            // Crossover:
            if (rnd.Rannyu() < 0.80) tsp.crossover(t1, t2);
            mutate(t1);
            mutate(t2);

            // Insert the two offspring into the new population
            if (new_population.size() < (size_t)n_population) new_population.insert({t1.distance_L1(), t1});
            if (new_population.size() < (size_t)n_population) new_population.insert({t2.distance_L1(), t2});

        }
        // update the population with the new population
        tsp.population = new_population;

        // write trip data to file
        tsp.save_trip(0, i+1, "../OUTPUT/best_trip_" + arg + ".dat");
        tsp.write_loss(i+1,   "../OUTPUT/best_loss_" + arg + ".dat");
        tsp.write_mean_loss(i+1, "../OUTPUT/mean_loss_" + arg + ".dat");

    }

    chrono::duration<double> dt = chrono::steady_clock::now() - t0; 
    cout << "Best L1: " << tsp.population.begin()->first << endl;
    cout << "Elapsed time: " << dt.count() << " s" << endl; 

    return 0;
}