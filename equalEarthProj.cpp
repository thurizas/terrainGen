#include "equalEarthProj.h"

equalEarthProj::equalEarthProj(float_t w, float_t h) : projection("equal earth"), m_winWidth(w), m_winHeight(h)
{

}

equalEarthProj::~equalEarthProj()
{

}

std::pair<double_t, double_t> equalEarthProj::convert(double_t lat, double lon)
{
  std::pair<double_t, double_t> ret;
  sf::Vector2f pt = forwardEqualEarth(static_cast<float_t>(lat), static_cast<float_t>(lon));

  ret.first = static_cast<double_t>(pt.x);
  ret.second = static_cast<double_t>(pt.y);
  
  return ret;
}


/**************************************************************************************************
 * Function: 
 *
 * Abstract: The scaling factor of 5.4 in the calculation of renderScale is derived from the fact
 *           that in the standard equal earth projection width is [-2.67, 2.67] units, or 5.34 units
 *           using 5.4 allows for a slight border around the map in the sfRenderWindow.
 *
 * Input   : lat - [in] the latitude of the point in radians, the range is [-pi/2, pi/2].  latitude
 *                      is represented by \phi in the wikipedia page.
 *           lon - [in] the longitude of the point in radians, the range is [-pi, pi].  longitude 
 *                      is represented by \lambda in the wikipedia page.
 *
 * Returns :
 *
 * Written : Nov 2026 (gkhuber) 
 *************************************************************************************************/
sf::Vector2f equalEarthProj::forwardEqualEarth(float_t lat, float_t lon)
{
  // 1. auxiliary parameter theta, \sin(\theta) = \frac{\sqrt(3)}{3}\sin(\phi}
  float_t theta = std::asin((sqrt3 / 2) * std::sin(lat));

  // 2. compute the powers of \theta in the polynominal equation
  float_t theta2 = theta * theta;
  float_t theta6 = theta2 * theta2 * theta2;
  float den = A1 + 3.0f * A2 * theta2 + 7.0f * A3 * theta6 + 9.0f * A4 * (theta6 * theta2);

  float x_proj = (2.0f * sqrt3 / 3.0f) * lon * std::cos(theta) / den;
  float y_proj = theta * (A1 + A2 * theta2 + A3 * theta6 + A4 * (theta6 * theta2));

  // map point coordinates to center of viewport
  float_t cx = m_winWidth / 2.0f;
  float_t cy = m_winHeight / 2.0f;

  float_t renderScale = m_winWidth / 5.4f; 

  return sf::Vector2f(cx + x_proj * renderScale, cy - y_proj * renderScale);
}