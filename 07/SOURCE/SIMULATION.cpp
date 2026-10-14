#include <iostream>
#include <string>

#include "system.h"
#include "utils.h"

using namespace std;

//values fixed after calibration:
const double delta_MC = 0.1029;
const double T_MD = 1.9612;
const double dt_MD = 0.001;
const double Tstar = 1.1;

const int n_inst = 500000;                // 7.2: 1 step per block
const int sim_nbl = 100, sim_L = 1000;    // 7.4: simulation with N blocks of L steps (value will be fixed after inst)
const int n_bins_g = 100;                     

int main(int argc, char* argv[]){
  string stage = (argc > 1) ? argv[1] : "";
  if(stage != "inst" && stage != "sim"){          
    cerr << "usage: ./simulation.exe [inst|sim]" << endl;
    return 1;
  }

  for(bool md : {false, true}){  
    const string tag   = md ? "MD" : "MC";
    const int    st    = md ? 0 : 1;
    const double delta = md ? dt_MD : delta_MC;

    equilibrate(md, tag, delta, md ? T_MD : Tstar);

    if(stage == "inst"){ // 7.2
      create_input_file(st, delta, Tstar, n_inst, 1, 1);
      //cout << "starting new simulation" << endl;
      run_simulation();
      keep("potential_energy.dat", "inst_U_" + tag + ".dat");  
      if(md) keep("temperature.dat", "inst_T_MD.dat");
    }
    else { // 7.4
      create_input_file(st, delta, Tstar, sim_nbl, sim_L, 1, {"PRESSURE", "GOFR " + to_string(n_bins_g)});
      run_simulation();
      for(string f : {"potential_energy", "pressure", "gofr"}) keep(f + ".dat", f + "_" + tag + ".dat");
      if(md){ keep("temperature.dat", "temperature_MD.dat"); keep("total_energy.dat", "total_energy_MD.dat"); }
      else    keep("acceptance.dat", "acceptance_MC.dat");
    }
  }
  return 0;
}