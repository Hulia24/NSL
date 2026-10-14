#include <iostream>
#include <fstream>
#include <functional>
#include <vector>
#include <algorithm>
#include <cmath>

#include "../LIBS/SA/quant.h"
#include "../LIBS/SA/Metropolis.h"
#include "../LIBS/SA/SA.h"
#include "../LIBS/DB/DataBlocking.h"
#include "../LIBS/PRNG/random.h"

using namespace std ;


int main () {

    Random rnd("../LIBS/PRNG/Primes","../LIBS/PRNG/seed.in" );

	//parameters of simulation:
    double T_max = 1;   
    double T_min = 0.001;
	double cooling_rate = 0.997;

	double delta = 2.5;
                                     
    const int n_blocks = 100 ;                                                  	
	const int block_size = 2000;
	int record_rate = 10;
	
	Metropolis walker;
	DataBlocking blocker;

	//inner chain for a parameter set:
	auto run_blocks = [&](const quant::Params state, int n_blocks, int block_size, ostream *coord){
		
		function<double(double)> pdf = [&](double x) {
			return quant::evalWaveFunctionSquared(x, state.mu, state.sigma);
		};

		walker.set_position(state.mu);
		walker.set_delta(delta*state.sigma);
		walker.equilibrate(rnd, pdf);

		vector<double> blocks(n_blocks);
		int step = 0;

		for(int j = 0; j<n_blocks; j++){
			double sum = 0.;
			for(int i = 0; i<block_size; i++, step ++){
				walker.step(rnd, pdf);
				sum += quant :: evalHamiltonian(walker.get_position(), state.mu, state.sigma);
				if (coord && step % record_rate == 0) *coord << step << "\t" << walker.get_position() << endl;
			}
			blocks[j] = sum/block_size;
		}

		return blocks;

	};

	//functions for Simulated Annealing:
	auto cost = [&](const quant::Params& state){
		auto blocks = run_blocks(state, n_blocks, block_size, nullptr);
		auto[mean, err] = blocker.GetFinalStats(blocks);
		return Energy{mean, err};
	};

	auto propose = [&](const quant::Params& state, double T, Random& rnd){

		double shrink = T/T_max;
		return quant::Params{fabs(state.mu + rnd.Gauss(0., 0.5 * shrink)), max(fabs(state.sigma +rnd.Gauss(0., 0.5 * shrink)), 0.01)};

	};

	//SA:
	SA<quant::Params> sa(rnd, quant::Params{1.0, 1.0}, propose, cost);
	cout << "Starting simulated annealing:" << endl;
	sa.cooling(T_max, T_min, cooling_rate, 1);
	cout << "Simulated annealing completed!" << endl;

	ofstream sa_file("OUTPUT/HamiltonianExpValue.dat");
	sa_file << "# STEP:  T:  MU:  SIGMA:  ENERGY:  ERROR:  BEST_ENERGY:" << endl;

	double running_best = sa.history().front().energy.value;
	int n = 0;
	for(const auto& run : sa.history()){
		running_best = min(running_best, run.energy.value);
		sa_file << n++ << "\t" << run.T << "\t" << run.state.mu << "\t" << run.state.sigma << "\t" << run.energy.value << "\t" << run.energy.error << "\t" << running_best << endl;
	}

	sa_file.close();

	const quant::Params best_params{sa.get_best_state().mu, sa.get_best_state().sigma};
	cout << "best parameters found: mu = " << best_params.mu << ", sigma = " << best_params.sigma << endl;

	//Hamiltonian with best parameters:

	const int final_iterations = 1000000;
    const int final_n_blocks = 100;
    const int final_block_size = final_iterations / final_n_blocks;

	ofstream coord("OUTPUT/GSCoordinates.dat");
	coord << "# STEP:  X_COORD:" << endl;

	auto blocks = run_blocks(best_params, final_n_blocks, final_block_size, &coord);
	coord.close();

	DataBlocking blocker_file("OUTPUT/GSHamiltonianExpValue.dat");
	blocker_file.AnalizeBlocksAverages(blocks);
	auto [mean, err] = blocker_file.GetFinalStats(blocks);

	cout << "<H> = " << mean << " +- " << err << endl;
    cout << "walker acceptance rate: " << walker.get_acceptance_rate() << endl;

	rnd.SaveSeed();

    return 0;
}