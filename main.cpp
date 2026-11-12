#include <SFML/Graphics.hpp>
#include <algorithm>
#include <string>
#include <vector>
#include <random>
#include <cmath>
#include <cctype>



#include "constants.h"
#include "console.h"
#include "logger.h"
#include "tectonics.h"
#include "gradicule.h"
#include "projection.h"
#include "equalEarthProj.h"
#include "Mollweide.h"

#ifdef __WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "XGetopt.h"
#else
#include <unistd.h>
#endif

#include <iostream>

bool isInteger(const char* bp);
void showVersion(const char*);
void showHelp(const char*);

enum projType:int16_t {UNDEFINED=-1, EQUALEARTH = 1, MOLLWEIDE = 2};

int main(int argc, char** argv)
{
	uint8_t   dbgLevel = CLogger::level::WARNING;
	uint8_t   cntPlates = NUM_PLATES;
	uint32_t  winWidth = WIDTH;
	uint32_t  winHeight = HEIGHT;
	int32_t   gridW = GRID_W;
	int32_t   gridH = GRID_H;
	uint32_t  seed = 0xFFFFFFFF;
    projType  proj = projType::EQUALEARTH;
	bool      viewPlates = true;
	bool      drawGradicule = true;

	int choice = -1;
	int ret = EXIT_SUCCESS;

	allocConsole();

	while (-1 != (choice = getopt(argc, argv, "dgpP:s:vh")))
	{
		switch (choice)
		{
		case 'd':                           // set debug level
			dbgLevel--;
			if (dbgLevel < CLogger::level::INFO) dbgLevel = CLogger::level::INFO;
			break;

		case 'p':                           // toggle view plates
			viewPlates = !viewPlates;
			break;

		case 'P':                           // select projection
		{
			std::string projName = std::string(optarg);
			std::transform(projName.begin(), projName.end(), projName.begin(), [](uint8_t ch) { return std::tolower(ch);});
			if(projName == "equalearth") {	proj = projType::EQUALEARTH;}
			else if(projName == "mollweide") {proj = projType::MOLLWEIDE;}
			else { proj = projType::UNDEFINED; }
		}
		break;		
	
		case 'g':                           // toggle draw gradicule
			drawGradicule = !drawGradicule;
			break;

		case 'v':
			showVersion(argv[0]);
			return(ret);

		case 's' :
			if(isInteger(optarg))
				seed = atoi(optarg);
			break;

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
	std::mt19937       gen;
	if(seed != 0xFFFFFFFF)    // use provided seed
		gen.seed(seed);
	else
		gen.seed(rd());       // use the random device to get a seed.

	projection* pproj = nullptr;
	// setup projection we are using...
	switch (proj)
	{
		case projType::EQUALEARTH: pproj = new equalEarthProj(winWidth, winHeight); break;
		case projType::MOLLWEIDE: pproj = new MollweideProj(winWidth, winHeight, 170); break;  // TODO : 170 determinded emperically so both projections are ~same size, need a better way
	}               
	gradicule         grad(pproj);


	sf::RenderWindow window(sf::VideoMode({winWidth, winHeight}), "Spherical Tectonics (Equal Earth Projection)");
	window.setFramerateLimit(60);

	
	tectonics         tecPlates(cntPlates, gridW, gridH, &gen);

	std::vector<sf::VertexArray> gradicule;
	int cntLatLines = 180.0f / grad.slices() + 1.0;   
	int cntLonLines = 360.0f / grad.segments();       
	sf::Color lineColor = sf::Color::Yellow;

	// build horizontal lines.
	for (int32_t row = 0; row < cntLatLines; row++)
	{
		sf::VertexArray va(sf::PrimitiveType::LineStrip); 

		for (int32_t col = 0; col <= cntLonLines; col++)
		{
			int ndx = row * (cntLonLines + 1) + col;
			auto [x, y] = grad.getPoint(ndx);
			va.append(sf::Vertex{ {static_cast<float_t>(y), static_cast<float_t>(x)}, lineColor });
		}
		gradicule.push_back(va);
	}
	// build vertical lines  
	for (int32_t col = 0; col < cntLonLines + 1; col++)
	{
		sf::VertexArray va(sf::PrimitiveType::LineStrip);

		for (int32_t row = 0; row < cntLatLines; row++)
		{
			uint32_t ndx = col + row * (cntLonLines + 1);
			auto [x, y] = grad.getPoint(ndx);
			va.append(sf::Vertex{ {static_cast<float_t>(y), static_cast<float_t>(x)}, lineColor });
		}

		gradicule.push_back(va);
	}

	sf::VertexArray vertexArray(sf::PrimitiveType::Triangles);
	vertexArray.resize(gridW * gridH * 6);

	while (window.isOpen())
	{
		while (const auto event = window.pollEvent())
		{
			if (event->is<sf::Event::Closed>()) window.close();
			if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
			{
				if (keyPressed->code == sf::Keyboard::Key::Space) { viewPlates = !viewPlates; puts("toggling plates"); }
				if(keyPressed->code == sf::Keyboard::Key::G) { drawGradicule = !drawGradicule; puts("toggling grid"); }
				if (keyPressed->code == sf::Keyboard::Key::Escape) window.close();
				//if (keyPressed->code == sf::Keyboard::Key::R)
				//{
				//	grid.assign(GRID_W * GRID_H, Cell{});
				//	initializeTectonics();
				//}
				// TODO : if window dimensions change should the projection class be notified - probably yes.
			}
		}

		tecPlates.updateState();

		// populate projected coordinate models
		for (int32_t y = 0; y < gridH; y++)
		{
			for (int32_t x = 0; x < gridW; x++)
			{
				int32_t ndx = y * gridW + x;
				cell c = tecPlates.getCell(ndx);

				sf::Color cellColor = sf::Color::Transparent;

				if (c.plateId != 0xFF)
				{
					if (viewPlates)
					{
						cellColor = tecPlates.getPlateColor(c.plateId);   
					}
					else
					{
						if (c.elevation < -0.1f) cellColor = sf::Color(12, 24, 82);         // Trench
						else if (c.elevation < 0.0f) cellColor = sf::Color(32, 80, 168);    // Ocean Shelf
						else if (c.elevation < 0.25f) cellColor = sf::Color(55, 125, 55);   // Plains
						else if (c.elevation < 0.60f) cellColor = sf::Color(115, 85, 55);   // Mountains
						else                          cellColor = sf::Color(240, 240, 245); // Glaciers
					}
				}

				// compute bounding corners
				sf::Vector2f p0 = {static_cast<float_t>(pproj->convert(c.lat, c.lon).first), static_cast<float_t>(pproj->convert(c.lat, c.lon).second)};
				sf::Vector2f p1 = {static_cast<float_t>(pproj->convert(c.lat, c.lon + (2.0f * PI / gridW)).first), static_cast<float_t>(pproj->convert(c.lat, c.lon + (2.0f * PI / gridW)).second)};
				sf::Vector2f p2 = {static_cast<float_t>(pproj->convert(c.lat - (PI / gridH), c.lon).first), static_cast<float_t>(pproj->convert(c.lat - (PI / gridH), c.lon).second)};
				sf::Vector2f p3 = {static_cast<float_t>(pproj->convert(c.lat - (PI / gridH), (2.0f * PI / gridW)).first),static_cast<float_t>(pproj->convert(c.lat - (PI / gridH), (2.0f * PI / gridW)).second) };
				
				int vertexNdx = ndx * 6;
				//// set up vertex array, CCW winding order
				vertexArray[vertexNdx + 0] = sf::Vertex{ p0, cellColor };
				vertexArray[vertexNdx + 1] = sf::Vertex{ p1, cellColor };
				vertexArray[vertexNdx + 2] = sf::Vertex{ p2, cellColor };
				vertexArray[vertexNdx + 3] = sf::Vertex{ p1, cellColor };
				vertexArray[vertexNdx + 4] = sf::Vertex{ p3, cellColor };
				vertexArray[vertexNdx + 5] = sf::Vertex{ p2, cellColor };
			}
		}

		window.clear(sf::Color(10, 12, 18));
		window.draw(vertexArray);

		if (drawGradicule)  // Z-order is determined by order of drawing, bug gradicule last so it is visible
		{
			for (sf::VertexArray va : gradicule)
			{
				window.draw(va);
			}
		}

		window.display();
	}

 	return ret;
}


/**********************************************************************************************************************
 * Function: isInteger
 *
 * Abstract:
 *
 * Input   :
 *
 * Returns :
 *
 * Written : Nov 2026(gkhuber)
 *********************************************************************************************************************/
bool isInteger(const char * bp)
{
	const char* ep = bp + strlen(bp);
	return ep == std::find_if(bp, ep, [](char ch) {return !std::isdigit(ch); });
}


/**********************************************************************************************************************
 * Function: showVersion
 *
 * Abstract: 
 *
 * Input   :
 *
 * Returns :
 *
 * Written : Nov 2026(gkhuber)
 *********************************************************************************************************************/
void showVersion(const char* name)
{
	std::cout << name << " A procedural terrain generator, based on plate tectonics.  Version " << static_cast<int>(MAJOR) << "." << static_cast<int>(MINOR) << "." << static_cast<int>(PATCH) << std::endl;
}


/**********************************************************************************************************************
 * Function: showHelp
 *
 * Abstract:
 *
 * Input   :
 *
 * Returns :
 *
 * Written : Nov 2026(gkhuber)
 *********************************************************************************************************************/
void showHelp(const char* name)
{
	std::cout << name << "A procedural terrain generator, based on plate tectonics.               " << std::endl;
	std::cout << "Usage: " << name << " [options]  \nThe options are:                             " << std::endl;
    std::cout << "d             increase the debug verbosity, can be used multiple times          " << std::endl;
	std::cout << "g             toggle display of grid (also works during execution)              " << std::endl;
	std::cout << "p             toggle display of plates (aslo works during execution)            " << std::endl;
	std::cout << "P             sets the projection for the 2D map.  Currently accepts one of:    " << std::endl;
	std::cout << "                  \"equalEath\", \"mollweide\"                                  " << std::endl;
	std::cout << "s <num>       use number as the PRNG engine seed, if not given use a random seed" << std::endl;
	std::cout << "v             displays program version, and then exits                          " << std::endl;
	std::cout << "h             displays a short usage screen (this screen) and then exits        " << std::endl;

}

