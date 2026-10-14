#pragma once

#include <functional>
#include <algorithm>
#include "../PRNG/random.h"

class Metropolis {

    private:
    double m_x;
    double m_delta;
    int m_proposed;
    int m_accepted;

    public:
    Metropolis(double x = 0.0, double delta = 1.0) : m_x(x), m_delta(delta), m_proposed(0), m_accepted(0) {}
    ~Metropolis() = default;

    void set_position(double x) { m_x = x;}
    void set_delta(double delta) { m_delta = delta;}
    double get_position() const {return m_x; }
    double get_delta() const {return m_delta; }

    void reset_counters() { m_proposed = 0; m_accepted = 0;}
    double get_acceptance_rate() const { return m_proposed > 0 ? static_cast<double>(m_accepted) / m_proposed : 0.0; }

    void step(Random& rnd, const std::function<double(double)>& pdf){
        
        m_proposed++;
        double x_new = m_x + rnd.Rannyu(-m_delta, m_delta);

        //probability from pdf:
        double p_old = pdf(m_x);
        double p_new = pdf(x_new);

        double p_metropolis = 1.0;
        if(p_old > 0.0 ) p_metropolis = std::min(1.0, p_new/p_old);

        if ( rnd.Rannyu() < p_metropolis ){
            m_x = x_new;
            m_accepted ++;
        }

    }
    
    void equilibrate(Random& rnd, const std::function<double(double)>& pdf, int steps = 20000){
        for(int i = 0; i < steps; i++){
            step(rnd, pdf);
        }

        reset_counters();
    }

};