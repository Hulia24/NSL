#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <vector>

#include "../LIBS/PRNG/random.h"
#include "../LIBS/DB/DataBlocking.h"

using namespace std;

int main(){

    int M = 100000;
    int N = 100;
    int L = M/N;

    if (M % N != 0) {
        cerr << "Error: M has to be a multiple of N" << endl;
        return 1;
    }
   
    Random rnd("../LIBS/PRNG/Primes","../LIBS/PRNG/seed.in" );

    // sampling with uniform distribution:
    {
        DataBlocking blocker_uni("uni_sampling.dat", L);
        for (int i = 0; i<M; i++) blocker_uni.AddMeasure(M_PI*0.5*cos(M_PI*0.5*rnd.Rannyu()));
    }

    // sampling using importance sampling:

    {
        DataBlocking blocker_imp("imp_sampling.dat", L);
        for (int i = 0; i<M; i++){
            double x = 1.0 - sqrt(1.0- rnd.Rannyu());
            blocker_imp.AddMeasure( (M_PI*0.5*cos(M_PI*0.5*x))/(2.0*(1.0-x)) );
        }
    }

    return 0;
}

