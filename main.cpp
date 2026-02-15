
#include "constants.h"
#include "terrainGen.h"
#include "console.h"
#include "logger.h"

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


/*
 * TODO: 
 *       (3) remove code to build triangular lattice -- remove grid points, replace by centroid points (?)
 *       (3) add code to locate default terrain, 
 *       (4) add code to locate centers of plates
 *       (5) add code to form plates - use vorouni regions to represent plates
 *       (6) triangulate each plate
 *       (7) make image properties dlg a fixed size.
 *       (8) generte initial terrain - simplex noise pattern  ?? meteroid crater?  circular with depth as parabolae -- rim also parabola
 */
int main(int argc, char** argv)
{
	int choice = -1;
	int ret = -1;

	allocConsole();

	while (-1 != (choice = getopt(argc, argv, "dvh")))
	{
		switch (choice)
		{
		case 'd':

			break;

		case 'v':
			showVersion(argv[0]);
			return(0);

		case '?':
			std::cout << "Unknown command line option " << optarg[optind] << std::endl;
			[[fallthrough]];
		case 'h':

			showHelp(argv[0]);
			return (0);
		}
	}

	CLogger* pLogger = CLogger::getInstance();
	pLogger->regOutDevice(cmdLine, cmdColorOut);
	pLogger->setLevel(CLogger::level::INFO);
	pLogger->outMsg(cmdLine, CLogger::level::SUCCESS, "initialized logging engine");

	QApplication a(argc, argv);

	terrainGen   mainWindow;
	mainWindow.show();

	ret = a.exec();

	pLogger->outMsg(cmdLine, CLogger::level::SUCCESS, "shutting down logging engine");
	pLogger->delInstance();
	deallocConsole();

	return ret;
}

void showVersion(const char* name)
{
	std::cout << name << " A procedural terrain generator, based on plat tectonics.  Version " << static_cast<int>(MAJOR) << "." << static_cast<int>(MINOR) << "." << static_cast<int>(PATCH) << std::endl;
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