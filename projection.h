#ifndef _projection_h_
#define _projection_h_

#include <string>
#include <vector>
#include <map>

class projection
{
public:
  projection(std::string s) : m_name(s) { }
  virtual ~projection() { }

  virtual std::pair<double_t, double_t> convert(double_t, double_t) = 0;  // convert from(lat, long) -> (x,y)
  

private:
  std::string                                    m_name;              // name of projection
  std::vector <std::pair<double_t, double_t>>    m_points;            // lat,long coordinates
  std::vector<std::pair<double_t, double_t>>     m_cartPts;           // x,y coordinates for drawing maps
};

#endif
