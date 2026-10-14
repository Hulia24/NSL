#include "GA.h"

GA :: GA(int n_cities, int n_population, bool circle_trip, Random& r): rnd(r), _n_cities(n_cities), _n_in_population(n_population)
{
    vector<double> x, y;
    if (circle_trip)
    {
        for (int i = 0; i < n_cities; i++)
        {
            double theta = rnd.Rannyu(0, 2 * M_PI);
            x.push_back(cos(theta));
            y.push_back(sin(theta));
        }
    }
    else
    {

        for (int i = 0; i < n_cities; i++)
        {
            x.push_back(rnd.Rannyu());
            y.push_back(rnd.Rannyu());
        }
    }

    std::string config = circle_trip ? "circle" : "square";
    write_cities_config(x, y, "cities_config_" + config + ".dat");

    // initialize population:
    for (int i = 0; i < n_population; i++)
    {

        vector<city> temp_cities;
        for (int j = 0; j < n_cities; j++)
        {
            temp_cities.push_back(city(j, x[j], y[j]));
        }

        // save the first city so is the same for every path and shuffle the rest:
        std::default_random_engine engine(static_cast<unsigned int>(rnd.Rannyu() * 1e6));
        std::shuffle(temp_cities.begin() + 1, temp_cities.end(), engine);

        trip t(n_cities, temp_cities);
        t.check();

        population.insert(make_pair(t.distance_L1(), t));
    }
};

GA :: GA(int n_cities, int n_population, std::string file_path, Random& r): rnd(r), _n_cities(n_cities), _n_in_population(n_population)
{
    // initialize population from file:
    ifstream filein(file_path);
    if (!filein)
    {
        cerr << "ERROR: file " << file_path << " could not be opened! " << endl;
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < n_population; i++)
    {
        trip t(n_cities, file_path, rnd);
        t.check();
        population.insert(make_pair(t.distance_L1(), t));
    }

    filein.close();
};


void GA :: write_cities_config(vector<double> x, vector<double> y, std::string file_path)
{

    ofstream fileout(file_path);
    if (!fileout)
    {
        cerr << "ERROR: file " << file_path << "could not be opened!" << endl;
        exit(EXIT_FAILURE);
    }

    // writes the number of cities:
    fileout << "#" << x.size() << endl;

    for (size_t i = 0; i < x.size(); i++)
    {
        fileout << x[i] << "\t" << y[i] << endl;
    }

    fileout.close();
};

void GA :: print_population(std::string file_path)
{

    ofstream fileout(file_path);
    if (!fileout)
    {
        cerr << "ERROR: file " << file_path << "could not be opened!" << endl;
        exit(EXIT_FAILURE);
    }

    // population is a multimap, so it is sorted by distance
    for (auto it = population.begin(); it != population.end(); it++)
    {
        fileout << "Distance: " << it->first << endl;
        for (int i = 0; i < _n_cities; i++)
        {
            fileout << it->second.cities[i].id << " ";
        }
        fileout << endl;
    }

    fileout.close();
};


vector<double> GA :: save_loss() {

    vector<double> loss;
    for (auto it = population.begin(); it != population.end(); it ++ ){
        loss.push_back(it ->first);
    }

    return loss;

};

void GA :: write_loss(int n_generation, std::string file_path)
{

    ofstream fileout(file_path, ios::app); // append so that it doesn't create a new file each time
    if (!fileout.is_open())
    {
        cerr << "Error opening file: " << file_path << endl;
        return;
    }

    auto it = population.begin();

    fileout << n_generation << " " << it->first << endl;
    fileout.close();
};

void GA :: write_mean_loss(int n_generation, std::string file_path)
{

    ofstream fileout(file_path, ios::app);
    if (!fileout.is_open())
    {
        cerr << "Error opening file: " << file_path << endl;
        return;
    }

    double mean = 0;
    int count = 0;
    int half = population.size() / 2;

    for (auto it = population.begin(); it != population.end() && count < half; ++it, ++count)
    {
        mean += it->first;
    }

    if (half > 0)
        mean /= half;

    fileout << n_generation << " " << mean << endl;
    fileout.close();
};

void GA :: save_trip(int i, int n_gen, std::string file_path)
{

    ofstream fileout(file_path, ios::app);
    if (!fileout.is_open())
    {
        cerr << "Error opening file: " << file_path << endl;
        return;
    }

    auto it = population.begin();
    std::advance(it, i);

    fileout << n_gen << " ";
    fileout << it->first << " ";

    for (int j = 0; j < _n_cities; ++j)
    {
        fileout << it->second.cities[j].id << " ";
    }

    fileout << it->second.cities[0].id << " "; //  to close the trip

    fileout << endl;
    fileout.close();
};

// selection algorithms:
trip GA :: select_individual_probability()
{ // select a trip based on it's loss function, it's good for the first generations only

    double sum_inv_loss = 0.; // the inverse of the loss makes the best choice the one with the lower loss
    for (auto it = population.begin(); it != population.end(); it++)
    {
        sum_inv_loss += 1. / it->first; // sum of the fitness functions
    }

    double p = rnd.Rannyu(0., sum_inv_loss);

    double acc = 0.;
    for (auto it = population.begin(); it != population.end(); it++)
    {
        acc += 1. / it->first;
        if (acc >= p)
        {
            return it->second;
        }
    }

    //take the last element in population
    return population.rbegin()->second;
};

trip GA :: select_individual_power(double p)
{
    int M = population.size();

    double r = rnd.Rannyu();                          // r in [0,1)
    int j = static_cast<int>(M * std::pow(r, p));     // power law to favor the best individuals
    if (j >= M) j = M - 1;

    auto it = population.begin();                     
    std::advance(it, j);
    return it->second;
};

// crossover:
void GA :: crossover(trip &t1, trip &t2)
{

    int cut_indx = int(rnd.Rannyu(1, t1._n_cities - 1));

    vector<city> child1 = t1.cities;
    vector<city> child2 = t2.cities;

    vector<bool> in_child1(_n_cities, false);
    vector<bool> in_child2(_n_cities, false);

    // save the first part of the trips:
    for (int i = 0; i < cut_indx; i++)
    {
        in_child1[t1.cities[i].id] = true;
        in_child2[t2.cities[i].id] = true;
    }

    // Creates the firs child:
    int indx1 = cut_indx;
    for (int i = 0; i < _n_cities; i++)
    {
        int id = t2.cities[i].id;

        if (!in_child1[id])
        {
            child1[indx1++] = t2.cities[i];
            in_child1[id] = true;

            if (indx1 >= _n_cities) break;
        }
    }

    // Creates the second child:
    int indx2 = cut_indx;
    for (int i = 0; i < _n_cities; i++)
    {
        int id = t1.cities[i].id;

        if (!in_child2[id])
        {
            child2[indx2++] = t1.cities[i];
            in_child2[id] = true;

            if (indx2 >= _n_cities) break;
        }
    }

    t1.cities = child1;
    t2.cities = child2;

    t1.check();
    t2.check();
};
