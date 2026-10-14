#pragma once

#include <iostream>
#include <cmath>

using namespace std;

namespace quant{

    inline double evalGauss (double x, double mu, double sigma){
        double sigma2 = pow(sigma,2);
        return exp(-(pow(x-mu,2)/(2*sigma2)));
    }


    inline double evalWaveFunction(double x, double mu, double sigma){

        double sigma2 = pow(sigma, 2);
        return exp(- pow(x - mu, 2) / (2 *  sigma2)) + exp(- pow(x + mu, 2) / (2 *  sigma2));

    }

    inline double evalWaveFunctionSquared(double x, double mu, double sigma){
        
        return pow( abs(evalGauss(x,mu,sigma) + evalGauss(x,-1*mu,sigma)), 2);

    }

    inline double evalWaveFunctionSecondDerivative(double x, double mu, double sigma){

        double minusExp = evalGauss(x,mu,sigma) ;
        double plusExp = evalGauss(x,-1*mu,sigma) ;
        double sigma2 = pow(sigma,2) ;

        return ( pow((x - mu)/sigma2, 2) - 1./sigma2 ) * minusExp + ( pow((x + mu)/sigma2, 2) -1./sigma2 )* plusExp;
    }


    inline double evalPotential(double x, double a = 1., double b = -2.5){
        return a * pow(x, 4) + b * pow(x, 2);
    }

    inline double evalHamiltonian(double x, double mu, double sigma) { //h_bar = 1 and m = 1

        double psi = evalWaveFunction(x, mu, sigma);
        if (fabs(psi) < 1e-5) {
            return 0.0; 
        }
        
        double eloc = (( -0.5 * evalWaveFunctionSecondDerivative(x, mu, sigma) ) / psi) + evalPotential(x);
        if (isnan(eloc) || isinf(eloc) || fabs(eloc) > 100.0) {
            return 0.0;
        }
        return eloc;
        

    }

    struct Params { double mu; double sigma; };
    
};
