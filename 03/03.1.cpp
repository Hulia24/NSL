#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <vector>
#include <algorithm>

#include "../LIBS/PRNG/random.h"
#include "../LIBS/DB/DataBlocking.h"

using namespace std;


int main(){

    int M = 100000;
    int N = 100;
    int L = M/N;

    double S0 = 100.0;
    double T = 1.0;
    double k = 100.0;
    double r = 0.1;
    double sigma = 0.25;

    
    Random rnd("../LIBS/PRNG/Primes","../LIBS/PRNG/seed.in" );

    //direct method:

    {
        DataBlocking blocker_call("./OUTPUT/call_direct.dat", L);
        DataBlocking blocker_put("./OUTPUT/put_direct.dat", L);

        for(int i=0; i<M; i++){
                
            double Z = rnd.Gauss(0,1);
            double ST = S0*exp((r - (0.5*sigma*sigma))*T + (sigma*Z*sqrt(T)));

            double call = exp(-r*T) * max(0.0, ST-k);
            double put = exp(-r*T) * max(0.0, k-ST);

            blocker_call.AddMeasure(call);
            blocker_put.AddMeasure(put);

        }

        blocker_call.Close();
        blocker_put.Close();

    }

    //discrete sampling:

    {
        DataBlocking blocker_call("./OUTPUT/call_discrete.dat", L);
        DataBlocking blocker_put("./OUTPUT/put_discrete.dat", L);

        
        double dt = T/100.0;

        for(int i=0; i<M; i++){

            double ST = S0; 

            for(int j=1; j<=100; j++){

                double Z = rnd.Gauss(0,1);
                ST = ST * exp((r - (0.5*sigma*sigma))*dt + sigma*Z*sqrt(dt));
                
            }

            double call = exp(-r*T) * max(0.0, ST-k);
            double put = exp(-r*T) * max(0.0, k-ST);

            blocker_call.AddMeasure(call);
            blocker_put.AddMeasure(put);

        }

        blocker_call.Close();
        blocker_put.Close();

    }

    return 0;
}


