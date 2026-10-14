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
   
    Random rnd("../LIBS/PRNG/Primes","../LIBS/PRNG/seed.in" );
    
    //progressive mean
    {
        DataBlocking blocker_mean("./OUTPUT/OUTPUT/data_mean.dat", L);
        for (int i = 0; i<M; i++) blocker_mean.AddMeasure(rnd.Rannyu());
    }

    //progressive variance

    {
        DataBlocking blocker_var("./OUTPUT/data_var.dat", L);
        for (int i = 0; i<M; i++) blocker_var.AddMeasure(pow((rnd.Rannyu()-0.5),2));
    }

    // chi test

    M = 100;
    int n = 10000;
    int E = static_cast<double>(n) / M;

    int n_chi2 = 100;

    ofstream fileout("./OUTPUT/data_chi.dat");
    if (!fileout.is_open()){
        cerr << "Error: the file has not been opend!" << endl;
        return 1;
    }

    fileout << "# j     chi2" << endl;
    fileout << fixed << setprecision(6);

    for(int j=0; j<n_chi2; j++){

        vector<int> conteggi(M, 0);

        for(int k=0; k<n; k++){

            double r = rnd.Rannyu();
            int bin = static_cast<int>(r * M);
            if (bin == M) bin = M - 1;

            conteggi[bin] ++;

        }

        double chi2 = 0.;
        for (int i=0; i<M; i++){
            
            chi2 += pow((conteggi[i]-E), 2) /E; 
        }

        fileout << setw(1) << j+1 
                << setw(20) << chi2 << endl;
        
    }

    fileout.close();


    return 0;
}

