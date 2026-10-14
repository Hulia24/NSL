
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

    int M = 10000;
    int N = 100;
    int L = M/N;

    int N_steps = 100;

    if (M % N != 0) {
        cerr << "Error: M has to be a multiple of N" << endl;
        return 1;
    }
   
    Random rnd("../LIBS/PRNG/Primes","../LIBS/PRNG/seed.in" );

    {
        // Discrete random walk:

        cout << "Starting discrete random walk sampling" << endl;

        ofstream fileout1 ("discrete_RW.dat");
        if(!fileout1.is_open()){
            cerr << "Error: file not opened!" << endl;
            return 1;
        }

        fileout1 << setw(1) << "#step" << setw(10) << "Squared_distance" << setw(15) << "error" << endl;

        DataBlocking blocker_dis;

        for(int i=0; i<=N_steps; i++){ 

            vector<double> mean_square_dist_block (N, 0.0);

            for(int j=0; j<N; j++){
                
                double square_distance = 0.0;

                for(int l=0; l<L; l++){
                
                    vector <double> x (3, 0.0); //position 
                    
                    for (int k=0; k<i; k++){ //RW to i-step
                        
                        int ind = int(rnd.Rannyu(0,3));
                        if(rnd.Rannyu()< 0.5) { x[ind]+=1; }
                        else x[ind] -=1;

                    }

                    square_distance += (x[0]*x[0]) + (x[1]*x[1]) + (x[2]*x[2]);

                }

                mean_square_dist_block[j] = square_distance / double(L);

            }

            auto [mean, error] = blocker_dis.GetFinalStats(mean_square_dist_block);

            fileout1 << setw(1) << i 
                     << setw(10) << mean 
                     << setw(15) << error << endl;

        }

        fileout1.close();       
        cout << "Finished discrete random walk sampling" << endl;

    }
    
    {
        // Continuous random walk:

        cout << "Starting continuous random walk sampling" << endl;

        ofstream fileout2 ("continuous_RW.dat");
        if(!fileout2.is_open()){
            cerr << "Error: file not opened!" << endl;
            return 1;
        }

        fileout2 << setw(1) << "#step" << setw(10) << "squared_distance" << setw(15) << "error" << endl;

        DataBlocking blocker_cont;

        for(int i=0; i<=N_steps; i++){

            vector<double> mean_square_dist_block (N, 0.0);

            for(int j=0; j<N; j++){
                
                double square_distance = 0.0;

                for(int l=0; l<L; l++){
                
                    vector <double> x (3, 0.0);
                    for (int k=0; k<i; k++){
                        
                        double phi = rnd.Rannyu(0,2*M_PI);
                        double costheta = rnd.Rannyu(-1, 1);
                        double sintheta = sqrt(1.0 - costheta*costheta);
                        
                        x[0] += sintheta * cos(phi);
                        x[1] += sintheta * sin(phi);
                        x[2] += costheta;

                    }

                    square_distance += (x[0]*x[0]) + (x[1]*x[1]) + (x[2]*x[2]);

                }

                mean_square_dist_block[j] = square_distance / double(L);

            }
            
            auto [mean, error] = blocker_cont.GetFinalStats(mean_square_dist_block);
            fileout2 << setw(1) << i 
                     << setw(10) << mean 
                     << setw(15) << error << endl;

        }

        fileout2.close();
        cout << "Finished continuous random walk sampling" << endl;

    }

    return 0;
}





