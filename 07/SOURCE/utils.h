#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <cstdlib>

#include "system.h"

using namespace std;

inline void create_input_file(int sim_type = 0, double delta = 0.001, double temp=1.1, int nblocks = 20, int nsteps = 1000, int restart = 0, vector<string> extra_props = {}) {
  ofstream out("../INPUT/input.dat"); 
  out << "SIMULATION_TYPE         " << sim_type << endl;
  out << "RESTART                 " << restart << endl;
  out << "TEMP                    " << temp << endl;
  out << "NPART                   108" << endl;
  out << "RHO                     0.8" << endl;
  out << "R_CUT                   2.5" << endl;
  out << "DELTA                   " << delta << endl;
  out << "NBLOCKS                 " << nblocks << endl;  //1000 for MD; 20 for MC
  out << "NSTEPS                  " << nsteps << endl;   //1 for MD; 1000 for MC
  out << "" << endl;
  out << "ENDINPUT" << endl;
  out.close();

  vector<string> props{"POTENTIAL_ENERGY"};
  for(const string& p : extra_props) props.push_back(p); 
  if(sim_type == 0) for(string p : {"KINETIC_ENERGY", "TEMPERATURE", "TOTAL_ENERGY"}) props.push_back(p);
  ofstream pr("../INPUT/properties.dat");
  for(const string& p : props) pr << p << "\n";
  pr << "ENDPROPERTIES\n";
}

inline double read_final_stat_from_file(const string& filename) {
    ifstream f_in(filename);
    if (!f_in.is_open()) {
        cerr << "ERROR: could not open: " << filename << endl;
        return 0.0;
    }

    string line, last_line;
    while (getline(f_in, line)) {
        if (!line.empty() && line.find_first_not_of(" \t\n\r") != string::npos) {
            last_line = line;
        }
    }
    f_in.close();

    if (last_line.empty()) return 0.0;

    stringstream ss(last_line);
    double blocco, stat;
    if (ss >> blocco >> stat) return stat;

    return 0.0;
}

inline void run_simulation() {
    System SYS;
    SYS.initialize();
    SYS.initialize_properties();
    SYS.block_reset(0);

    for (int i = 0; i < SYS.get_nbl(); i++) {
        for (int j = 0; j < SYS.get_nsteps(); j++) {
            SYS.step();
            SYS.measure();
        }
        SYS.averages(i + 1);
        SYS.block_reset(i + 1);
    }

    SYS.finalize();
}

//copy a file with a new name:
inline void keep(const string& file, const string& newname){ system(("cp ../OUTPUT/" + file + " ../OUTPUT/" + newname).c_str()); }

inline void equilibrate(bool MD, const string& tag, double delta, double temp, int nbl = 100, int nst = 100) {
  system("cp ../INPUT/CONFIG/config.fcc ../INPUT/CONFIG/config.xyz");
  create_input_file(MD ? 0 : 1, delta, temp, nbl, nst, 0);
  run_simulation();
  keep("potential_energy.dat", "eq_U_" + tag + ".dat");
  keep(MD ? "temperature.dat" : "acceptance.dat", MD ? "eq_T_MD.dat" : "eq_acc_MC.dat");
  system("cp ../OUTPUT/CONFIG/config.xyz ../INPUT/CONFIG/config.xyz"); 
  system("cp ../OUTPUT/CONFIG/conf-1.xyz ../INPUT/CONFIG/conf-1.xyz");   
}