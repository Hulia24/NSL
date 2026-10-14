#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include "system.h"

using namespace std;


// Overwrites the input file with the new T and h.
void write_input(double temp, double h, int simtype) {
    ofstream out("../INPUT/input.dat");
    // SIMULATION_TYPE: 2 (or 3 for Gibbs), J=1.0, H=h
    out << "SIMULATION_TYPE         " << simtype << "   1.0    " << h << endl;
    out << "RESTART                 0" << endl;
    out << "TEMP                    " << temp << endl;
    out << "NPART                   50" << endl;
    out << "RHO                     1.0" << endl;
    out << "R_CUT                   0.0" << endl;
    out << "DELTA                   0.0" << endl;
    out << "NBLOCKS                 20" << endl;
    out << "NSTEPS                  20000" << endl;
    out << "" << endl;
    out << "ENDINPUT" << endl;
    out.close();
}

int main() {
    double T_min = 0.5, T_max = 2.0;
    const int  n_points = 16, n_eq = 5000, nbl = 20, nsteps = 20000;
    double dT = (T_max - T_min) / double(n_points - 1);
    const string names[] = {"", "", "metropolis", "gibbs"}; //to match simtype (indx = simtype)


    for(int simtype =2; simtype <=3; simtype++){
        for(double h : {0.0, 0.02}){
            string filename = "../OUTPUT/final_" + names[simtype] + (h == 0.0 ? "_h0.dat" : "_h002.dat");
            ofstream fout(filename);
            fout << (h == 0.0 ? "# T   E  err   cv  err   chi  err" : "# T   M  err") << endl;

            write_input(T_max, h, simtype);
            System SYS;
            SYS.initialize();
            SYS.initialize_properties();

            for(int i = 0; i < n_points; i++){
                double T = T_max - i*dT;
                SYS.set_temperature(T);
                SYS.reset_averages();

                for(int j = 0; j < n_eq; j++) SYS.step();   //riequilibration for each T
                for(int k = 1; k <= nbl; k++){
                    SYS.block_reset(k);
                    for(int s = 0; s < nsteps; s++){ SYS.step(); SYS.measure(); }
                    SYS.averages(k);
                }

                double ave, err;
                fout << fixed << setprecision(6) << T;

                if(h == 0.0){
                    for(string p : {"tenergy", "cv", "chi"}){
                        SYS.get_results(p, nbl, ave, err);
                        fout << " " << ave << " " << err;
                    }
                } else {
                    SYS.get_results("magnet", nbl, ave, err);
                    fout << " " << ave << " " << err;
                }

                fout << endl;
                cout << names[simtype] << " simulation with h=" << h << "and T=" << T << " done" << endl;
                
            }
        }
    }

    return 0;
}