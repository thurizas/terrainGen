// see https://en.wikipedia.org/wiki/Equal_Earth_projection for the conversions for latitude 
// and longitude to cartesian coordinates

#ifndef _equalearthproj_h_
#define _equalearthproj_h_

#include "projection.h"
#include "constants.h"
#include "logger.h"

class equalEarthProj : public projection
{
public:
  equalEarthProj(float_t winWidth, float_t winHeight);
  ~equalEarthProj();

  std::pair<double_t, double_t> convert(double_t lat, double_t);

  sf::Vector2f forwardEqualEarth(float_t lat, float_t lon);

private:
  float_t     m_winWidth;
  float_t     m_winHeight;
};

#endif

