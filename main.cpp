#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include <cmath>



#include "constants.h"
#include "console.h"
#include "logger.h"
#include "tectonics.h"
#include "gradicule.h"
#include "projection.h"
#include "equalEarthProj.h"

#ifdef __WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "XGetopt.h"
#else
#include <unistd.h>
#endif

#include <iostream>
#include <QApplication>

void showVersion(const char*);
void showHelp(const char*);



int main(int argc, char** argv)
{
	uint8_t   cntPlates = NUM_PLATES;
	uint32_t  winWidth = WIDTH;
	uint32_t  winHeight = HEIGHT;
	int32_t   gridW = GRID_W;
	int32_t   gridH = GRID_H;
	bool      viewPlates = false;

	int choice = -1;
	int ret = EXIT_SUCCESS;

	allocConsole();

	while (-1 != (choice = getopt(argc, argv, "dvh")))
	{
		switch (choice)
		{
		case 'd':

			break;

		case 'v':
			showVersion(argv[0]);
			return(ret);

		case '?':
			std::cout << "Unknown command line option " << optarg[optind] << std::endl;
			[[fallthrough]];
		case 'h':
			showHelp(argv[0]);
			return (ret);
		}
	}

	CLogger* pLogger = CLogger::getInstance();
	pLogger->regOutDevice(cmdLine, cmdColorOut);
	pLogger->setLevel(CLogger::level::INFO);
	pLogger->outMsg(cmdLine, CLogger::level::SUCCESS, "initialized logging engine");

	std::random_device rd;
	std::mt19937       gen(rd());

	// setup projection we are using...
	equalEarthProj    proj(winWidth, winHeight);                
	gradicule         grad(&proj);


	sf::RenderWindow window(sf::VideoMode({winWidth, winHeight}), "Spherical Tectonics (Equal Earth Projection)");
	window.setFramerateLimit(60);

	
	tectonics         tecPlates(cntPlates, gridW, gridH, &gen);

	sf::VertexArray vertexArray(sf::PrimitiveType::Triangles);
	vertexArray.resize(gridW * gridH * 6);

	while (window.isOpen())
	{
		while (const auto event = window.pollEvent())
		{
			if (event->is<sf::Event::Closed>()) window.close();
			if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
			{
				if (keyPressed->code == sf::Keyboard::Key::Space) viewPlates = !viewPlates;
				//if (keyPressed->code == sf::Keyboard::Key::R)
				//{
				//	grid.assign(GRID_W * GRID_H, Cell{});
				//	initializeTectonics();
				//}
				// TODO : if window dimensions change should the projection class be notified - probably yes.
			}
		}

		tecPlates.updateState();

		//// populate projected coordinate models
		//for (int32_t y = 0; y < gridH; y++)
		//{
		//	for (int32_t x = 0; x < gridW; x++)
		//	{
		//		int32_t ndx = y * gridW + x;
		//		cell c = tecPlates.getCell(ndx);

		//		sf::Color cellColor = sf::Color::Transparent;

		//		if (c.plateId != 0xFF)
		//		{
		//			if (viewPlates)
		//			{
		//				cellColor = tecPlates.getPlateColor(c.plateId);   
		//			}
		//			else
		//			{
		//				if (c.elevation < -0.1f) cellColor = sf::Color(12, 24, 82);         // Trench
		//				else if (c.elevation < 0.0f) cellColor = sf::Color(32, 80, 168);    // Ocean Shelf
		//				else if (c.elevation < 0.25f) cellColor = sf::Color(55, 125, 55);   // Plains
		//				else if (c.elevation < 0.60f) cellColor = sf::Color(115, 85, 55);   // Mountains
		//				else                          cellColor = sf::Color(240, 240, 245); // Glaciers
		//			}
		//		}

		//		// compute bounding corners
		//		sf::Vector2f p0 = proj.forwardEqualEarth(c.lat, c.lon);
		//		sf::Vector2f p1 = proj.forwardEqualEarth(c.lat, c.lon + (2.0f * PI / gridW));
		//		sf::Vector2f p2 = proj.forwardEqualEarth(c.lat - (PI / gridH), c.lon);
		//		sf::Vector2f p3 = proj.forwardEqualEarth(c.lat - (PI / gridH), (2.0f * PI / gridW));

		//		int vertexNdx = ndx * 6;

		//		// set up vertex array, CCW winding order
		//		vertexArray[vertexNdx + 0] = sf::Vertex{ p0, cellColor };
		//		vertexArray[vertexNdx + 1] = sf::Vertex{ p1, cellColor };
		//		vertexArray[vertexNdx + 2] = sf::Vertex{ p2, cellColor };
		//		vertexArray[vertexNdx + 3] = sf::Vertex{ p1, cellColor };
		//		vertexArray[vertexNdx + 4] = sf::Vertex{ p3, cellColor };
		//		vertexArray[vertexNdx + 5] = sf::Vertex{ p2, cellColor };
		//	}
		//}

		// setup grid 



		window.clear(sf::Color(10, 12, 18));
		// setup grid - draw horizontal lines
		// points 0 - 11   0 to 2\pi/\DeltaLon - 1 
		// point 12 - 23
		// 13 row: 180/15 = 12
		for (int row = 0; row <= 180 / 15; row++)
		{
			std::cout << "drawing row " << row << " using points " << row * 12 << " to " << row * 12 + 11 << std::endl;
		}


		//window.draw(vertexArray);
		window.display();
	}

 	return ret;
}

void showVersion(const char* name)
{
	std::cout << name << " A procedural terrain generator, based on plate tectonics.  Version " << static_cast<int>(MAJOR) << "." << static_cast<int>(MINOR) << "." << static_cast<int>(PATCH) << std::endl;
}

/**********************************************************************************************************************
 * Function: 
 *
 * Abstract:
 *
 * Input   :
 *
 * Returns :
 *
 * Written : () 
 *********************************************************************************************************************/
void showHelp(const char* name)
{
	std::cout << name << "A procedural terrain generator, based on plate tectonics." << std::endl;
	std::cout << "Usage: " << name << " [options]  \nThe options are:" << std::endl;
	std::cout << "v             displays program version, and then exits" << std::endl;
	std::cout << "h             displays a short usage screen (this screen) and then exits" << std::endl;

}