
#include <sstream>
#include "trip.h"

#ifndef NDEBUG
    #define TRIP_DEBUG_CHECK() check()
#else
    #define TRIP_DEBUG_CHECK() ((void)0)
#endif
    
trip :: trip( int n_cities, string file_path, Random& rand) {
        
    _n_cities = n_cities;
    int count = 0;

    cities.resize(n_cities);
    for(int i=0; i<_n_cities; i++){
        cities[i].id = i;
        cities[i].x = 0.;
        cities[i].y = 0.;
    }

    //get cities position from file:
    std::ifstream filein(file_path);
    if (!filein) {
        std::cerr << "Error opening file: " << file_path << std::endl;
        std::exit(EXIT_FAILURE);
    }

    std::string line;
    while (count < _n_cities && getline(filein, line)) {

        if (line.empty() || line[0] == '#') continue;

        std::stringstream ss(line);
        if (ss >> cities[count].x >> cities[count].y) count++;
    }

    if(count != _n_cities){
        std::cerr << "Error: Number of cities read from file does not match expected number." << std::endl;
        std::exit(EXIT_FAILURE);
    }

    check();

    // Random initial tour. The engine is seeded from the (rank-dependent)
    // custom generator so every MPI rank starts from a different tour.
    std::mt19937 engine(static_cast<unsigned int>(rand.Rannyu() * 1e6));
    std::shuffle(cities.begin() + 1, cities.end(), engine);

    check();


};


double trip :: distance_L1(){ // Sum of Euclidean distances |x_i - x_{i+1}|
    double d = 0;

    for (int i = 0; i < _n_cities - 1; i++) {
        d += std::hypot(cities[i].x - cities[i+1].x, cities[i].y - cities[i+1].y);
    }

    // closing the path to the starting point:
    d += std::hypot(cities[_n_cities - 1].x - cities[0].x, cities[_n_cities - 1].y - cities[0].y);

    return d;
};

   
vector<int> trip :: get_path() {
        
    std::vector<int> path;
    path.reserve(_n_cities);
    for (const auto& c : cities) path.push_back(c.id);
    return path;
};

void trip :: check() {  //checks if all the cities are unique :
        
    std::map<int, bool> visited;
        
    for (int i = 0; i < _n_cities; i++) {
            
        if (visited.find(cities[i].id) != visited.end()) {
            std::exit(EXIT_FAILURE); 
        }
        visited[cities[i].id] = true;
    }
};
    

// mutations:
    
void trip :: random_swap_mutation(Random &rnd){

    int first_index = int(rnd.Rannyu(1, _n_cities));
    int second_index = int(rnd.Rannyu(1, _n_cities));

    while (first_index == second_index) second_index = int(rnd.Rannyu(1, _n_cities));

    swap(cities[first_index], cities[second_index]);

    TRIP_DEBUG_CHECK();
};

void trip :: shift_mutation(Random &rnd, int block_length, int n_shift ){

    if (block_length < 1 || n_shift < 1 || block_length >= _n_cities - 2 || n_shift > _n_cities - 2) return;

    int max_start = _n_cities - block_length;
    if (max_start < 1)return;

    int start_indx = int(rnd.Rannyu(1, max_start + 1));

    std::vector<city> temp(cities.begin() + 1, cities.end());
    std::vector<city> block(temp.begin() + (start_indx - 1), temp.begin() + (start_indx - 1 + block_length));

    temp.erase(temp.begin() + (start_indx - 1), temp.begin() + (start_indx - 1 + block_length));

    int insert_pos = (start_indx - 1 + n_shift) % (_n_cities - block_length);
    temp.insert(temp.begin() + insert_pos, block.begin(), block.end());

    for (int i = 1; i < _n_cities; ++i) cities[i] = temp[i - 1];

    TRIP_DEBUG_CHECK();
};

void trip :: inversion_mutation(Random &rnd, int block_length){

    if (block_length < 1 || block_length >= _n_cities - 1) return;

    int max_start = _n_cities - block_length;
    if (max_start < 1) return;

    int start_indx = int(rnd.Rannyu(1, max_start + 1));
    int end_indx = start_indx + block_length;

    std::reverse(cities.begin() + start_indx, cities.begin() + end_indx);

    TRIP_DEBUG_CHECK();
};
    
void trip :: block_permutation_mutation(Random &rnd, int block_length){

    if (block_length < 1 || block_length > (_n_cities - 1) / 2) return;

    int max_start_1 = _n_cities - (2 * block_length);
    int start_indx_1 = int(rnd.Rannyu(1, max_start_1 + 1));
    int end_1 = start_indx_1 + block_length;

    int min_start_2 = end_1;
    int max_start_2 = _n_cities - block_length;
    int start_indx_2 = int(rnd.Rannyu(min_start_2, max_start_2 + 1));

    std::swap_ranges(cities.begin() + start_indx_1, cities.begin() + end_1, cities.begin() + start_indx_2);

    TRIP_DEBUG_CHECK();
};