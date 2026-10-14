#pragma once

#include <cmath>
#include <vector>
#include <functional>
#include <utility>
#include "../PRNG/random.h"

struct Energy { double value = 0.0; double error = 0.0; };


template <class State> //State is the set of parameters
class SA {

    using Proposed_function = std::function<State(const State&, double T, Random&)>;
    using Cost_function    = std::function<Energy(const State&)>;
    struct Record { double T; State state; Energy energy; };

    private:
    Random& m_rnd;
    Proposed_function m_proposed;
    Cost_function m_cost;
    State m_current, m_best;
    Energy m_current_e{0., 0.}, m_best_e{0., 0.};
    std::vector<Record> m_history;
    int m_n_proposed{0}, m_n_accepted{0};

    public:

    SA(Random& rnd, State s0, Proposed_function f, Cost_function cost)
    : m_rnd(rnd), m_proposed(std::move(f)), m_cost(std::move(cost)), m_current(s0), m_best(m_current)
    { 
        m_current_e = m_cost(m_current);  
        m_best_e = m_current_e;
    }

    //One metropolis step on the parameters (mu, sigma) a temperature T:
    bool step(double T){

        State candidate = m_proposed(m_current, T, m_rnd);
        Energy E = m_cost(candidate);

        double delta = E.value - m_current_e.value;
        bool accepted = (delta <= 0.0) || (m_rnd.Rannyu() < std::exp(-delta/T));

        m_n_proposed ++;
        if(accepted){
            
            m_n_accepted++;
            m_current = std::move(candidate); m_current_e = E;

            if(E.value < m_best_e.value){m_best = m_current; m_best_e = E;}

        }

        return accepted;

    }

    //geometric cooling:
    void cooling(double T_max, double T_min, double cooling_rate, int record_rate){

        m_history.clear();
        int n = 0;
        for(double T = T_max; T > T_min; T *= cooling_rate){
            step(T);
            if (n++ % record_rate == 0) m_history.push_back({T, m_current, m_current_e});
        }
    }

    const State& get_current_state() const {return m_current; }
    const State& get_best_state() const {return m_best; }
    Energy get_current_energy() const { return m_current_e; }
    Energy get_best_energy() const { return m_best_e; }
    double acceptance_rate() const {return m_n_proposed ? double(m_n_accepted)/m_n_proposed : 0.0;}
    const std::vector<Record>& history() const { return m_history; }
    
};