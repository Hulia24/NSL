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

    Random rnd;
    if (!rnd.Initialize("../LIBS/PRNG/Primes","../LIBS/PRNG/seed.in" )) {
        cerr << "Error in the initialization of the numeber generator" << endl;
        return 1;
    }

    double l = 2.;
    double d = 2.5;

    int N_thr = 100000;

    int L_block = 1000;
    int N_blocks = N_thr/L_block;
    
    DataBlocking blocker("./OUTPUT/Buffon.dat", 1); //I'll compute the mean Pi every loop so L is 1

    for (int i=0; i< N_blocks; i++){
        int N_hit = 0;
       
        for(int j=0; j<L_block; j++){
            // needle's centre:
            double y_c = rnd.Rannyu(0, d/2.);
            
            // needle's rotation
            double x, y, r2;
            do{
                x = rnd.Rannyu();
                y = rnd.Rannyu();
                r2 = x*x + y*y;
            } while ( r2 > 1 || r2 == 0);
            
            double sin = y/sqrt(r2);

            // the needle will intersect the y=0 line if (y_c - sin) <= 0:
            if ( y_c <= 0.5*l*sin ) N_hit ++ ;

        }

        if (N_hit > 0){

            double pi_estimate = (2. * l  * L_block)/( d * N_hit );
            blocker.AddMeasure(pi_estimate);
        }

    }

    return 0;
}