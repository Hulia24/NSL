#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <string>
#include <vector>
#include <mpi.h>

#include "../../LIBS/PRNG/random.h"
#include "../../LIBS/GA/PT.h"
#include "../INPUT/input.h"

using namespace std;

int main( int argc, char* argv[]){

    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    InputParameters params = read_input("../INPUT/input.dat");

    //initialize solver:
    Random rnd("../../LIBS/PRNG/Primes", "../../LIBS/PRNG/seed.in", rank);
    
    double T_rank;
    if (size > 1) T_rank = params.t_min * pow(params.t_max / params.t_min, static_cast<double>(rank) / (size - 1)); //exponential scaling
    else T_rank = params.t_min;

    string filename = params.cities_file;
    PT tsp(params.n_cities, filename, rnd);
    tsp.set_temperature(T_rank);

    ofstream out_loss("../OUTPUT/loss_" + params.geometry + "_rank_" + to_string(rank) + ".dat");
    out_loss << "# step  T  current_loss  best_loss\n";

    double t_start = MPI_Wtime();

    //PARALLEL TEMPERING MAIN LOOP:
    for(int step = 1; step <= params.n_steps; step ++){

        //local Metropolis step:
        tsp.Metro_step();
        if(step % 100 == 0) out_loss << step << "\t" << tsp.get_temperature() << "\t"  << tsp.get_current_loss() << "\t" << tsp.get_best_loss() << "\n";

        //Swaps:
        tsp.PT_swap(step, params.swap_freq);
    }

    out_loss.close();

    //COLLECTING RESULTS:
    double local_best_loss = tsp.get_best_loss();
    double global_best_loss = 0.0;

    // global minimum loss:
    MPI_Allreduce(&local_best_loss, &global_best_loss, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);
    int best_rank = (fabs(local_best_loss - global_best_loss) < 1e-9) ? rank : size;
    
    //communicating best rank:
    int global_best_rank = size;
    MPI_Allreduce(&best_rank, &global_best_rank, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);
    
    // Swap statistics:
    int local_proposed = tsp.get_proposed_swaps();
    int local_accepted = tsp.get_accepted_swaps();
    int total_proposed = 0;
    int total_accepted = 0;

    MPI_Reduce(&local_proposed, &total_proposed, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(&local_accepted, &total_accepted, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    double time = MPI_Wtime() - t_start;

    // Per-pair swap statistics: index r -> pair (r, r+1)
    vector<int> all_prop(size), all_acc(size);
    MPI_Gather(&local_proposed, 1, MPI_INT, all_prop.data(), 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Gather(&local_accepted, 1, MPI_INT, all_acc.data(),  1, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        double rate = (total_proposed > 0) ? static_cast<double>(total_accepted) / total_proposed : 0.0;

        cout << "Global Best Loss L1: " << global_best_loss << " (found by rank " << global_best_rank << ")" << endl;
        cout << "PT swap acceptance rate: " << fixed << setprecision(4) << rate << endl;
        cout << "Elapsed time: " << setprecision(3) << time << " s" << endl;
    }

    //best rank saves file:
    if (rank == global_best_rank) {
        
        ofstream out_best_path("../OUTPUT/best_path_" + params.geometry + ".dat");
        
        const vector<city>& best_cities = tsp.get_best_trip().cities;
        for (const auto& c : best_cities) out_best_path << c.x << "\t" << c.y << "\n";
        
        //to close the path:
        if (!best_cities.empty()) out_best_path << best_cities[0].x << "\t" << best_cities[0].y << "\n";
        
        out_best_path.close();
        rnd.SaveSeed();
    }

    MPI_Finalize();
    

    return 0;
}