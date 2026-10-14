#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

#include "system.h"
#include "utils.h"

using namespace std;

int main() {

    // Delta calibration:
    cout << "Starting delta calibration for MC: " << endl;

    vector<double> deltas;
    for (double d = 0.05; d <= 0.3; d += 0.025) deltas.push_back(d);

    ofstream outFileMC("../OUTPUT/CALIBRATION/acceptance_vs_delta.dat");
    outFileMC << "# DELTA   ACCEPTANCE" << endl;

    for (double delta : deltas) {
    
        ofstream clear("../OUTPUT/acceptance.dat", ios::trunc);
        clear.close();

        system("cp ../INPUT/CONFIG/config.fcc ../INPUT/CONFIG/config.xyz 2>/dev/null");

        create_input_file(1, delta, 1.1, 20, 1000, 0);
        run_simulation();

        double acc = read_final_stat_from_file("../OUTPUT/acceptance.dat");
        outFileMC << fixed << setprecision(6) << delta << " " << acc << endl;
        cout << "delta = " << delta << " | acceptance = " << acc << endl;
    }

    outFileMC.close();


    // Temperature calibration:
    cout << "Starting temperature calibration for MD: " << endl;

    vector<double> T_starts;
    for (double t = 1.9; t <= 2.0; t += 0.01) T_starts.push_back(t);

    ofstream outFileMD("../OUTPUT/CALIBRATION/temp_correlation_md.dat");
    outFileMD << "# T_start   T_actual" << endl;

    for (double t_start : T_starts) {
    
        ofstream clear("../OUTPUT/temperature.dat", ios::trunc);
        clear.close();

        system("cp ../INPUT/CONFIG/config.fcc ../INPUT/CONFIG/config.xyz 2>/dev/null");

        create_input_file(0, 0.001, t_start, 1000, 1, 0);
        run_simulation();

        double t_finale =  read_final_stat_from_file("../OUTPUT/temperature.dat");     
        outFileMD << fixed << setprecision(6) << t_start << " " << t_finale << endl;
        cout << "MD -> Tested T_start: " << t_start << " | Resulting T_final: " << t_finale << endl;
    }

    outFileMD.close();

    cout << "Calibration finished" << endl;
    

    return 0;
}