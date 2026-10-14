#include "PT.h"

PT :: PT(int n_cities, const std::string& namefile, Random& rnd)
    : _current_trip(n_cities, namefile, rnd), 
      _best_trip(_current_trip),  
      _current_loss(_current_trip.distance_L1()),
      _best_loss(_current_loss),
      _temp(0.0), 
      _rnd(rnd),
      _proposed_swaps(0),
      _accepted_swaps(0)
{
    MPI_Comm_rank(MPI_COMM_WORLD, &_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &_size);

    if (_current_trip.distance_L1() == 0.0){
        std::cerr << "Error: initial trip has lenght zero!" << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

};

bool PT :: SA_step(){

    const int n_cities = _current_trip.get_n_cities();
    trip proposal = _current_trip;

    //final trip mutation:
    const double prob = _rnd.Rannyu();
    const int max_block = std::max(2, std::min(10, n_cities/4));

    if (prob < 0.20) proposal.random_swap_mutation(_rnd);
    else if (prob < 0.35){
        int block = int(_rnd.Rannyu(1, max_block));
        int shift = int(_rnd.Rannyu(1, max_block));
        proposal.shift_mutation(_rnd, block, shift);
    }
    else if (prob < 0.45){
        int block = int(_rnd.Rannyu(1, max_block));
        proposal.block_permutation_mutation(_rnd, block);
    }
    else {
        //Lenght drawn from an exponential law:
        int block = 2 + int(-std::log(1.0 - _rnd.Rannyu()) * n_cities / 8.0);
        block = std::min(block, n_cities - 2);
        proposal.inversion_mutation(_rnd, block);
    }

    double proposal_loss = proposal.distance_L1();
    double delta = proposal_loss - _current_loss;

    //Metropolis acceptance:
    bool acceptance = false;
    if(delta <= 0.0 ) acceptance = true;
    else if (_rnd.Rannyu() < std::exp(-delta / _temp)) acceptance = true;
    

    if(acceptance){
        _current_trip = proposal;
        _current_loss = proposal_loss;

        if(_current_loss < _best_loss){
            _best_loss = _current_loss;
            _best_trip = _current_trip;
        }
    }

    return acceptance;

};

bool PT :: PT_swap(int step, int swap_freq){

    if(_size <= 1 or step % swap_freq != 0 ) return false;

    //Choose partner according to the parity of the attempt:
    const int parity = (step/swap_freq)%2;
    const int partner = ((_rank % 2) == parity) ? _rank + 1 : _rank - 1;

    //For ranks at the edge:
    if (partner < 0 || partner >= _size) return false;

    bool swapped = false;
    const bool is_master = (_rank < partner);

    //Exchange (loss, temperature) with partner:
    double rank[2] = { _current_loss, _temp};
    double other[2];
    MPI_Sendrecv(rank,  2, MPI_DOUBLE, partner, 0, other, 2, MPI_DOUBLE, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE );
    
    //Master rank decides the outcome:
    if(is_master){
        
        const double beta_master = 1./_temp;
        const double beta_slave = 1./other[1];
        const double loss_master = _current_loss;
        const double loss_slave = other[0];

        const double delta = (beta_master - beta_slave) * (loss_master - loss_slave);
        swapped = (delta >= 0.0 || _rnd.Rannyu() < std::exp(delta)) ? true : false;

        _proposed_swaps++;
        if(swapped) _accepted_swaps++;

        MPI_Send(&swapped, 1, MPI_CXX_BOOL, partner, 1, MPI_COMM_WORLD);
    } else MPI_Recv(&swapped, 1, MPI_CXX_BOOL, partner, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    if(!swapped) return false;

    //swap paths:
    const int n_cities = _current_trip.get_n_cities();
    std::vector<double> send_buffer(3*n_cities), recv_buffer(3*n_cities);

    for(int i=0; i<n_cities; i++){
        send_buffer[3*i]  = _current_trip.cities[i].id;
        send_buffer[3*i + 1] = _current_trip.cities[i].x;
        send_buffer[3*i + 2] = _current_trip.cities[i].y;
    }

    MPI_Sendrecv(send_buffer.data(), 3*n_cities, MPI_DOUBLE, partner, 2, recv_buffer.data(), 3*n_cities, MPI_DOUBLE, partner, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    for(int i = 0; i < n_cities; i++){
        _current_trip.cities[i].id = static_cast<int>(recv_buffer[3 * i]);
        _current_trip.cities[i].x = recv_buffer[3*i + 1];
        _current_trip.cities[i].y = recv_buffer[3*i + 2];
    }

    _current_loss = _current_trip.distance_L1();
    if(_current_loss < _best_loss){
        _best_loss = _current_loss;
        _best_trip = _current_trip;
    }
    
    return swapped;

};





