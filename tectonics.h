#ifndef _tectonics_h_
#define _tectonics_h_

#include "constants.h"
#include <cstdint>
#include <random>



class plate
{
public:
	plate();

	static uint8_t genID() { return m_plateCnt++; }

	uint8_t id() { return m_id; }
	void rotAxis(sf::Vector3f a) { m_rotationalAxis = a; }
	sf::Vector3f rotAxis() { return m_rotationalAxis; }
	void rotSpeed(float_t s) { m_rotationalSpeed = s;}
	float_t rotSpeed() { return m_rotationalSpeed; }
	void color(sf::Color c) { m_color = c; }
	sf::Color color() { return m_color; }

	friend std::ostream& operator<<(std::ostream, const plate&);

private:
	static uint8_t m_plateCnt;
	
	// state of the plate...
	uint8_t        m_id;
	sf::Vector3f   m_rotationalAxis;
	float_t        m_rotationalSpeed;
	sf::Color      m_color;
};


class tectonics
{
public:
	tectonics(uint8_t, int32_t, int32_t, std::mt19937*);
	~tectonics();

	void updateState(float_t);

	void resetTime() { m_elapsedTime = 0.0f; }
	struct cell getCell(int32_t);
	float_t getElapsedTime() { return m_elapsedTime; }
	float_t getDeltaTime() { return m_dt; }

	sf::Color getPlateColor(int32_t);

private:
	uint8_t                  m_nbrPlates;
	std::vector<plate>       m_plates;            // vector of plate structures
	std::vector<int>         m_seeds;             // vector of seed location for Voronoi flood fill
	std::vector<struct cell> m_grid;              // cells on sphere
	int32_t                  m_gridW;
	int32_t                  m_gridH;
	std::mt19937*            m_pGen;
	float_t                  m_elapsedTime = 0.0f; // in million years
	float_t                  m_dt = 0.5f;          // 1.0 million years per step

	void initGrid();
	void initTectonics();
	void initPlates();


};

#endif

