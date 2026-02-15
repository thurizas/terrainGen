/*
 * we use a generalized extreme value distribution with parameters mu=12.9183, sigma = 2.37793, xi = -1.0
 * we use the method described at https://stackoverflow.com/questions/28744065/generalized-extreme-value-distribution-in-c-gev
 * to generate the random variables.  (C++ methods only provide for two parameters: mu, sigma), the code in the above link
 * allows us to use all three variables.
 */

#pragma once

#include <random>

class CGEVDist
{
public:
    CGEVDist();
    CGEVDist(float, float, float);
    ~CGEVDist();

    float  getNumber();

private:
    float                         m_mu;
    float                         m_sigma;
    float                         m_xi;
    std::default_random_engine    m_generator;
    //std::uniform_real_distribution<double> m_distribution(0,1);

    double invCDF(float);
};


