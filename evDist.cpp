#include "evDist.h"

#include "qDebug"

CGEVDist::CGEVDist() { }


CGEVDist::CGEVDist(float mu, float sigma, float xi) : m_mu(mu), m_sigma(sigma), m_xi(xi)
{ 
    


}

CGEVDist::~CGEVDist()
{  }



float CGEVDist::getNumber()
{
       

  std::uniform_real_distribution<double> distribution(0,1);
  double number = distribution(m_generator);
  
  qDebug() << "seed number is: " << number;

  return  invCDF(number);

}



double CGEVDist::invCDF(float x)
{
  if(m_xi == 0)
  {
    return (m_mu-m_sigma*log(-log(x)));
  }
  else
  {
    double a = pow(-1*log(x), -1*m_xi);
    double b = -1*m_xi*m_mu*pow(-log(x),m_xi);
    double c = m_sigma*pow(-log(x),m_xi) - m_sigma;
    return (-1)*(a*(b+c))/m_xi;
  }
}





/*
#include <iostream>
#include <random>
#include <cmath>

double icdf(double x, double mu, double sigma, double xi)
{
  if(xi == 0)
  {
    return (mu-sigma*log(-log(x)));
  }
  else
  {
    double a = pow(-1*log(x), -1*xi);
    double b = -1*xi*mu*pow(-log(x),xi);
    double c = sigma*pow(-log(x),xi) - sigma;
    return (-1)*(a*(b+c))/xi;
  }
}



int main(int argc, char** argv)
{
    std::default_random_engine    generator;
  
    std::uniform_real_distribution<double> distribution(0,1);
 
    int  p[nintervals] = {};

  for(int i = 0; i < nrolls; ++i)
  {
    double number = distribution(generator);
    number = icdf(number, 12.9183, 3.37793, -1.0);
    std::cout << number << " ";
    if((number >= -5.0) && (number <20.0))
      ++p[int(number-first)];
  }


  std::cout << "extreme_value_distribution (13.9183, 2.7793):" << std::endl;

  for (int i = 0; i < nintervals; ++i)
  {
    std::cout.width(2); std::cout << float(first+i) << " ..";
    std::cout.width(2); std::cout << float(first+i+1) << ":";
    std::cout << std::string(p[i]*nstars/nrolls, '*') << std::endl;
  }

  return 0;
}
*/
