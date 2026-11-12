#ifndef _gradicule_h_
#define _gradicule_h_

#include "projection.h"

const float deltaLon = 30;
const float deltaLat = 15;

class gradicule
{
public: 
  gradicule(projection* proj, float dlat=15 , float dlon=30);
  ~gradicule();

  float  slices() { return m_deltaLat; }
  float  segments() { return m_deltaLon; }
  size_t size() { return m_pts.size(); }
  std::pair<double_t, double_t> getPoint(int32_t ndx) { return m_mapPts[ndx]; }
  std::pair<double_t, double_t> getWorldPoint(int32_t ndx) { return m_pts[ndx]; }

private:
  projection* m_proj = nullptr;
  float       m_deltaLat;
  float       m_deltaLon;

  std::vector<std::pair<double_t, double_t>>   m_pts;
  std::vector<std::pair<double_t, double_t>>   m_mapPts;

  void worldGradicule();
  void mapGradicule();

};


#endif

