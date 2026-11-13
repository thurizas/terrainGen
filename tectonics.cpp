#include "tectonics.h"

#include <iostream>

uint8_t plate::m_plateCnt = 0;

plate::plate() : m_id(genID())
{

}




tectonics::tectonics(uint8_t cnt, int32_t w, int32_t h, std::mt19937* pgen) : m_nbrPlates(cnt), m_gridW(w), m_gridH(h), m_pGen(pgen)
{
  initGrid();
  initTectonics();
  initPlates();
}

tectonics::~tectonics()
{

}

struct cell tectonics::getCell(int32_t ndx)
{
  int32_t cntCells = m_gridW * m_gridH;
  if ((ndx < 0) || (ndx > m_gridW * m_gridH))
  {
    return cell{ 0xFF, 0.0f, 0.0f };
  }
  else
  {
    return m_grid[ndx];
  }
}

sf::Color tectonics::getPlateColor(int32_t plateId)
{
  sf::Color clr = sf::Color::Transparent;

  if((plateId >= 0) && (plateId < m_plates.size()))
  { 
    clr = m_plates[plateId].color();
  }

  return clr;
}


void tectonics::initGrid()
{
  m_grid.resize(m_gridW * m_gridH);

  for (int32_t y = 0; y < m_gridH; ++y)
  {
    float_t v = static_cast<float_t>(y) / (m_gridH);
    float_t lat = PI / 2.0f - v * PI;

    for (int32_t x = 0; x < m_gridW; ++x)
    {
      float_t u = static_cast<float_t>(x) / (m_gridW);
      float_t lon = u * 2.0f * PI - PI;

      int32_t idx = y * m_gridW + x;
      m_grid[idx].lat = lat;
      m_grid[idx].lon = lon;
    }
  }
}


void tectonics::initTectonics()
{
  std::uniform_real_distribution<float_t> distAxis(-1.0f, 1.0f);
  std::uniform_real_distribution<float_t> distSpeed(1.5f, 10.0f);                    // realisting tectonic movement is 1.5 cm/year to 10.1 cm/year
  std::uniform_int_distribution<int>      distGrid(0, ((m_gridW * m_gridH) - 1));

  for (uint8_t ndx = 0; ndx < m_nbrPlates; ndx++)
  {
    plate   newPlate;

    sf::Vector3f axis(distAxis(*m_pGen), distAxis(*m_pGen), distAxis(*m_pGen));      // randomly choose an euler-axis
    float_t   mag = std::sqrt(axis.x * axis.x + axis.y * axis.y + axis.z * axis.z);

    newPlate.rotAxis(sf::Vector3f{axis.x/mag, axis.y/mag, axis.z/mag});
    newPlate.rotSpeed(distSpeed(*m_pGen));
    newPlate.color(sf::Color{ static_cast<uint8_t>(40 + (ndx * 30) % 210), 
                              static_cast<uint8_t>(40 + (ndx * 75) % 210), 
                              static_cast<uint8_t>(55 + (ndx * 110) % 200) });
    m_plates.push_back(newPlate);

    int seedIdx = distGrid(*m_pGen);
    m_grid[seedIdx].plateId = newPlate.id();
    m_grid[seedIdx].elevation = 0.2f;

    m_seeds.push_back(seedIdx);
  }
}

/**************************************************************************************************
 * Function: initPlates
 *
 * Abstract: Performs a spherical flood fill, simulating a spherical Voronoi cell algorithm.
 *
 * Input   :
 *
 * Returns :
 *
 * Written : Dec 2026 (gkhuber) 
 *************************************************************************************************/
void tectonics::initPlates()
{
  std::vector<int> queue = m_seeds;
  while (!queue.empty())
  {
    std::vector<int> next;

    int32_t elemNbr = 0;

    for (int ndx : queue) 
    {
      int currentId = m_grid[ndx].plateId;

      int cx = ndx % m_gridW;
      int cy = ndx / m_gridW;

      sf::Vector2i neighbors[] = { {cx + 1, cy}, {cx - 1, cy}, {cx, cy + 1}, {cx, cy - 1} };
      for (const auto& n : neighbors)
      {
        // azmuthal wrapping west -> east, east -> west
        int wx = (n.x + m_gridW) % m_gridW;

        // polar wrapping, clamp at 90 degrees north and south.
        int wy = n.y;
        if (wy < 0 || wy >= m_gridH) continue;

        int idx = wy * m_gridW + wx;
        if (m_grid[idx].plateId == 0xFF)
        {
          m_grid[idx].plateId = currentId;
          next.push_back(idx);
        }
      }
    }
    queue = next;
  }
}

void tectonics::updateState(float_t dt)
{
  //m_elapsedTime += m_dt;
  std::vector<struct cell> nextGrid = m_grid;

  for (int32_t ndx = 0; ndx < m_gridH * m_gridW; ndx++)
  {
    uint8_t  pId = m_grid[ndx].plateId;
    if (0xFF == pId) continue;

    plate Plate = m_plates[pId];

    float_t omega = (Plate.rotSpeed() * CMPERYR2KMPERMYR) / EARTH_RADIUS;    // convert linear speed into angular speed
    float_t deltaAngle = omega * dt;                                         // calculate angular change

    float_t cosLat = std::cos(m_grid[ndx].lat);
    float_t posX = cosLat * std::cos(m_grid[ndx].lon);
    float_t posY = cosLat * std::sin(m_grid[ndx].lon);
    float_t posZ = std::sin(m_grid[ndx].lat);

    // Apply Rodrigues' rotation formula to find the advection displacement
    sf::Vector3f crossProd(
      Plate.rotAxis().y * posZ - Plate.rotAxis().z * posY,
      Plate.rotAxis().z * posX - Plate.rotAxis().x * posZ,
      Plate.rotAxis().x * posY - Plate.rotAxis().y * posX);

    // compute new position vector
    float_t nextX = posX + crossProd.x * deltaAngle;
    float_t nextY = posY + crossProd.y * deltaAngle;
    float_t nextZ = posZ + crossProd.z * deltaAngle;

    // convert back to lat, lon coordinates
    float newLat = std::asin(nextZ / std::sqrt(nextX * nextX + nextY * nextY + nextZ * nextZ));
    float newLon = std::atan2(nextY, nextX);

    // transform lat,lon into array indexes
    int targetX = static_cast<int>(((newLon + PI) / (2.0f * PI)) * (m_gridW - 1));
    int targetY = static_cast<int>(((PI / 2.0f - newLat) / PI) * (m_gridH - 1));

    targetX = std::clamp(targetX, 0, m_gridW - 1);
    targetY = std::clamp(targetY, 0, m_gridH - 1);
    int targetIdx = targetY * m_gridW + targetX;

    // Boundary stress checks
    if (m_grid[targetIdx].plateId != pId && m_grid[targetIdx].plateId != -1) {
      int targetPlateId = m_grid[targetIdx].plateId;

      // Tectonic compression: plates moving toward one another
      float dotMove = (Plate.rotAxis().x * m_plates[targetPlateId].rotAxis().x) +
        (Plate.rotAxis().y * m_plates[targetPlateId].rotAxis().y) +
        (Plate.rotAxis().z * m_plates[targetPlateId].rotAxis().z);

      if (dotMove < 0.5f)         // Convergent boundary collision
      {
        nextGrid[targetIdx].elevation += 0.08f;
        nextGrid[ndx].elevation += 0.04f;
      }
      else                        // Divergent boundary rifting
      {
        nextGrid[ndx].elevation -= 0.05f;
      }
    }
    else                          // Passive internal transport
    {
      
      nextGrid[targetIdx].elevation += m_grid[ndx].elevation * 0.005f;
      nextGrid[ndx].elevation -= m_grid[ndx].elevation * 0.005f;
    }
  }

  m_grid = nextGrid;
  m_elapsedTime += dt;
}


