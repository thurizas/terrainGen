#ifndef _gradicule_h_
#define _gradicule_h_

#include "projection.h"

const float deltaLon = 30;
const float deltaLat = 15;

class gradicule
{
public: 
  gradicule(projection* proj);
  ~gradicule();


private:
  projection* m_proj = nullptr;

  std::vector<std::pair<double_t, double_t>>   m_pts;
  std::vector<std::pair<double_t, double_t>>   m_mapPts;

  void worldGradicule();
  void mapGradicule();

};


#endif

