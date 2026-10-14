#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <mpi.h>

struct InputParameters {
    std::string geometry;
    std::string cities_file;
    int n_cities = 0;
    int n_steps = 0;
    int swap_freq = 10;
    double t_min = 0.01;
    double t_max = 1.0;
};

inline InputParameters read_input(const std::string& filename) {
    
    InputParameters params;
    std::ifstream filein(filename);

    if (!filein.is_open()) {
        std::cerr << "ERROR! could not open file " << filename << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    std::string line, key;
    while (std::getline(filein, line)) {
        
        if (line.empty() || line[0] == '#') continue;

        std::stringstream ss(line);
        key.clear();
        if (!(ss >> key)) continue; 

        if      (key == "GEOMETRY")     ss >> params.geometry;
        else if (key == "CITIES_FILE")  ss >> params.cities_file;
        else if (key == "N_CITIES")     ss >> params.n_cities;
        else if (key == "N_STEPS")      ss >> params.n_steps;
        else if (key == "SWAP_FREQ")    ss >> params.swap_freq;
        else if (key == "T_MIN")        ss >> params.t_min;
        else if (key == "T_MAX")        ss >> params.t_max;
    }

    filein.close();

    if (params.n_cities < 4 || params.n_steps < 1 || params.swap_freq < 1 || params.t_min <= 0.0 || params.t_max < params.t_min || params.cities_file.empty()) {
        std::cerr << "ERROR! invalid or missing parameters in " << filename << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    return params;
}