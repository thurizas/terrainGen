#include "gradicule.h"
#include "constants.h"


#include <iostream>


gradicule::gradicule( projection* p, float dlat, float dlon) : m_proj(p), m_deltaLat(dlat), m_deltaLon(dlon)
{
  worldGradicule();                        // build world gradicule, (lat,lon) of intersections
  mapGradicule();                          // build map gradicule (x,y) in current projection
}

gradicule::~gradicule()
{

}

/**********************************************************************************************************************
 * Function: worldGradicule
 *
 * Abstract: This builds the grid system in world coordinates (most likely on a unit sphere).  The coordinates are 
 *           built in 'row-order' form, i.e. all the points for 90 degrees north latitude, then 90-deltaLat, and so on.  
 *           There will be 2\pi/\DeltaLon points per row.
 *           The range for latitude is [0, 180] and the range for longidude is [=180, 180).  angles are stored in 
 *           radians not degrees.  We add the point at (+180, ?) to make building lines easier in the plotting section
 *           For example if deltaLon = 30 and deltaLat = 15 the points generated would be,
 *   0: (-180, 90),(-150, 90),(-120, 90),(-90, 90),(-60, 90),(-30, 90),(0, 90),(30, 90),(60, 90),(90, 90),(120, 90),(150, 90),(180, 90)
 *  13: (-180, 75),(-150, 75),(-120, 75),(-90, 75),(-60, 75),(-30, 75),(0, 75),(30, 75),(60, 75),(90, 75),(120, 75),(150, 75),(180, 75)
 *  26: (-180, 60),(-150, 60),(-120, 60),(-90, 60),(-60, 60),(-30, 60),(0, 60),(30, 60),(60, 60),(90, 60),(120, 60),(150, 60),(180, 60)
 *  39: (-180, 45),(-150, 45),(-120, 45),(-90, 45),(-60, 45),(-30, 45),(0, 45),(30, 45),(60, 45),(90, 45),(120, 45),(150, 45),(180, 45)
 *  52: (-180, 30),(-150, 30),(-120, 30),(-90, 30),(-60, 30),(-30, 30),(0, 30),(30, 30),(60, 30),(90, 30),(120, 30),(150, 30),(180, 30)
 *  65: (-180, 15),(-150, 15),(-120, 15),(-90, 15),(-60, 15),(-30, 15),(0, 15),(30, 15),(60, 15),(90, 15),(120, 15),(150, 15),(180, 15)
 *  78: (-180,  0),(-150,  0),(-120,  0),(-90,  0),(-60,  0),(-30,  0),(0,  0),(30,  0),(60,  0),(90,  0),(120,  0),(150,  0),(180,  0)
 *  91: (-180,-15),(-150,-15),(-120,-15),(-90,-15),(-60,-15),(-30,-15),(0,-15),(30,-15),(60,-15),(90,-15),(120,-15),(150,-15),(180,-15)
 * 104: (-180,-30),(-150,-30),(-120,-30),(-90,-30),(-60,-30),(-30,-30),(0,-30),(30,-30),(60,-30),(90,-30),(120,-30),(150,-30),(180,-30)
 * 117: (-180,-45),(-150,-45),(-120,-45),(-90,-45),(-60,-45),(-30,-45),(0,-45),(30,-45),(60,-45),(90,-45),(120,-45),(150,-45),(180,-45)
 * 130: (-180,-60),(-150,-60),(-120,-60),(-90,-60),(-60,-60),(-30,-60),(0,-60),(30,-60),(60,-60),(90,-60),(120,-60),(150,-60),(180,-60)
 * 143: (-180,-75),(-150,-75),(-120,-75),(-90,-75),(-60,-75),(-30,-75),(0,-75),(30,-75),(60,-75),(90,-75),(120,-75),(150,-75),(180,-75)
 * 156: (-180,-90),(-150,-90),(-120,-90),(-90,-90),(-60,-90),(-30,-90),(0,-90),(30,-90),(60,-90),(90,-90),(120,-90),(150,-90),(180,-90)
 * 
 *
 * Input   :
 *
 * Returns :
 *
 * Written : Nov 2026 (gkhuber) 
 *********************************************************************************************************************/
void gradicule::worldGradicule()
{
  for (int32_t lat = 90; lat >= -90; lat -= m_deltaLat)
  {
    for (int32_t lon = -180; lon <= 180; lon += m_deltaLon)
    {
      m_pts.push_back({ lon*DEG2RAD, lat*DEG2RAD });
    }
  }
}



/**********************************************************************************************************************
 * Function: rebuild
 *
 * Abstract: erases the old map coordinates and then rebuilds them based on the new window size.
 *
 * Input   :
 *
 * Returns :
 *
 * Written : Nov 2026 (gkhuber)
 *********************************************************************************************************************/
void gradicule::rebuild()
{
  // TODO : erase m_mapPts;
  m_mapPts.erase(m_mapPts.begin(), m_mapPts.end());
  mapGradicule();
}


/**********************************************************************************************************************
 * Function: mapGradicule
 *
 * Abstract: converts the world gradicule points into map coordinates under the current projection.
 *
 * Input   :
 *
 * Returns :
 *
 * Written : Nov 2026 (gkhuber)  
 *********************************************************************************************************************/
 void gradicule::mapGradicule()
{
   if (m_proj != nullptr)
   {
     for (auto [lon, lat] : m_pts)
     {
       std::pair<double_t, double_t> xformCoords = m_proj->convert(lat, lon);
       m_mapPts.push_back({ xformCoords.second, xformCoords.first });
     }
   }
}
