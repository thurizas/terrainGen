#include "Mollweide.h"
#include "constants.h" 

#include <iostream>

/* converts from latitude and longitude to x- and y- coordinates via the following 
 * equations:
 *
 *       R*2*sqrt(2)
 *  x = ------------ (l - l_0)cos(t)                   -2R*sqrt(2) <= x <= 2R*sqrt(2)
 *         PI
 *
 *   y = R*sqrt(2)*sin(t)                              -R*sqrt(2) <= y <= R*sqrt(2)
 *
 *   where 2*t + sin(2*t) = PI*sin(p)                  auxilliary equation.
 *
 * where l is the longitude, l_0 is the central meridian, p is the latitude and R is radius of the globe.
 * and t is an auxillary angle.
 *
 * The auxilliary equation can be solved by Newton-Raphson iteration.
 *
 *     if latitude is +/- pi/2  then t is +/- pi/2
 *
 *     else     t_0 = lat
 *              t_{n+1} = t_n - ((2*t_n + sin(2*t_n) - pi*sin(lat))/(2 + 2*cos(2*t_n)))
 *
 */
MollweideProj::MollweideProj(float_t w, float_t h, float r ) : projection("Mollweide"), m_winWidth(w), m_winHeigth(h), m_fRadius(r), m_fCoeff(r* sqrt(2.0))
{
}


MollweideProj::~MollweideProj(void)
{
}


/* precondition : expect lat and lon to be passed in as radians */
std::pair<double_t, double_t> MollweideProj::convert(double_t lat, double_t lon)
{
    double_t theta;                                                      // solution to auxillary equation....

    if((fabs(lat - PI/2) < EPSILON) || (fabs(PI/2 + lat) < EPSILON))     // latitude is \pm \pi/2 radians skip iteration and set theta to \pm \pi/2
    { 
        theta = lat;
    }
    else
    {
        
        theta = solveAux(lat, lat);              // iterate on auxillary equation
    }

    double_t x = 2*m_fCoeff * lon * cos(theta) / PI;
    double_t y = m_fCoeff * sin(theta);
   
    float_t cx = m_winWidth / 2.0f;
    float_t cy = m_winHeigth / 2.0f; 
    float_t renderScale = m_winWidth / 5.8; // 5.4f;

      //return sf::Vector2f(cx + x_proj * renderScale, cy - y_proj * renderScale);
     return std::pair<double_t, double_t>(cx + x ,cy - y);
 }


// solve with Newton-Raphson using iteration
double_t MollweideProj::solveAux(double_t lat_r, double_t guess)
{
    double_t newguess;

    newguess = guess - ((2*guess + sin(2*guess) - PI*sin(lat_r))/(2 + 2*cos(2*guess)));

    if(fabs(guess - newguess) <= 200* EPSILON)
    {
        return newguess;
    }
    else
        return solveAux(lat_r, newguess);
}







