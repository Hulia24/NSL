#include <iostream>
#include <fstream>
#include <functional>
#include <string>
#include <vector>
#include <cstdlib>

#include "../LIBS/SA/quant.h"
#include "../LIBS/SA/Metropolis.h"
#include "../LIBS/DB/DataBlocking.h"
#include "../LIBS/PRNG/random.h"

using namespace std ;


int main (int argc, char *argv[]) {

	if(argc < 3 ) {
		cerr << " ERROR: mu and sigma are required " << endl;
		return 1;
	}

    Random rnd("../LIBS/PRNG/Primes","../LIBS/PRNG/seed.in" );
       
	// Define parameters of simulation
	const quant :: Params state{atof(argv[1]), atof(argv[2])};

	// monte carlo variables
	double delta = 2.5;

	// data blocking variables
    const int iterations = pow(10,6) ;
    const int n_blocks = 200 ;
	const int block_size = (int)(iterations/n_blocks);
	int record_rate = 1000;

	// Output files
	const string tag = string(argv[1]) + "_" + string(argv[2]);
    const string ham_file   = "OUTPUT/HamiltonianExpValue_" + tag + ".dat";
    const string coord_file = "OUTPUT/Coordinates_" + tag + ".dat";

	// |psi_T|^2 for these parameters
	function<double(double)> pdf = [&](double x) {
        return quant::evalWaveFunctionSquared(x, state.mu, state.sigma);
    };

	Metropolis walker(state.mu, delta*state.sigma);
	walker.equilibrate(rnd, pdf);

	ofstream coord(coord_file);
	coord << "# STEP:  X_COORD:" << endl;

	vector<double> blocks(n_blocks);
	int step = 0;

	for(int j = 0; j<n_blocks; j++){
		double sum = 0.;
		for(int i = 0; i<block_size; i++, step++){
			walker.step(rnd, pdf);
			sum += quant :: evalHamiltonian(walker.get_position(), state.mu, state.sigma);
			if (step % record_rate  == 0) coord << step << "\t" << walker.get_position() << endl;
		}
		blocks[j] = sum/block_size;
	}

	coord.close();

	DataBlocking blocker(ham_file);
    blocker.AnalizeBlocksAverages(blocks);
    auto [mean, err] = blocker.GetFinalStats(blocks);

    cout << "mu = " << state.mu << "  sigma = " << state.sigma << endl;
    cout << "<H> = " << mean << " +- " << err << endl;
    cout << "acceptance rate: " << walker.get_acceptance_rate() << endl;

    rnd.SaveSeed();

    return 0;
}