#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <vector>

#include "../LIBS/PRNG/random.h"

using namespace std;

int main(){

   vector<int> N = {1, 2, 10, 100};
   int throws  = 10000;

   Random rnd("../LIBS/PRNG/Primes","../LIBS/PRNG/seed.in"); 

   for (int i = 0; i < int(N.size()); i++){

     ofstream out_uni("./OUTPUT/uni_" + to_string(N[i]) + ".dat");
     ofstream out_exp("./OUTPUT/exp_" + to_string(N[i]) + ".dat");
     ofstream out_lnz(".OUTPUT/lor_" + to_string(N[i]) + ".dat");

      for (int j = 0; j < throws; j++){
         double Sn_uni = 0.0;
         double Sn_exp = 0.0;
         double Sn_lnz = 0.0;

            for (int k = 0; k < N[i]; k++){
                Sn_uni += int(rnd.Rannyu(1,7));   
                Sn_exp += rnd.Exponential(1);
                Sn_lnz += rnd.Cauchy_Lorentz(0,1);
            }

         out_uni << Sn_uni/N[i] << endl;
         out_exp << Sn_exp/N[i] << endl;
         out_lnz << Sn_lnz/N[i] << endl;
      }

     out_uni.close();
     out_exp.close();
     out_lnz.close();
   } 

 
   return 0;
}