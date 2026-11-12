#ifndef _constants_h_
#define _constants_h_

#include <cstdint>
#include <SFML/Graphics.hpp>

static const uint8_t MAJOR = 0;
static const uint8_t MINOR = 2;
static const uint16_t PATCH = 0;

// Mathematical and physical constants....
static const double epsilion = DBL_EPSILON;
static const double PI = 3.1415926535897932384626433832795;
static const double DEG2RAD = PI / 180.0;
static const double RAD2DEG = 180.0 / PI;
static const double sqrt2 = 1.4142135623730950488016887242097;
static const double sqrt3 = 1.7320508075688772935274463415059;
static const double sin30 = 0.5000000000000000000000000000000;      // sin30 = 1/2
static const double cos30 = 0.8660254037844386467637231707529;      // cos30 = sqrt(3)/2
static const double sin60 = 0.8660254037844386467637231707529;      // sin60 = sqrt(3)/2
static const double cos60 = 0.5000000000000000000000000000000;      // cos30 = 1/2

// equal earth constants....
const float A1 = 1.340264f;
const float A2 = -0.081106f;
const float A3 = 0.000893f;
const float A4 = 0.003796f;

// window and simulation default values
const int WIDTH = 1028;
const int HEIGHT = 500;                         // Equal Earth aspect ratio is ~2.05:1
const int SCALE = 2;                            // Resolution scaling factor
const int GRID_W = WIDTH / SCALE;
const int GRID_H = HEIGHT / SCALE;

const int NUM_PLATES = 9;



struct cell
{
	uint8_t      plateId = 0xFF;                   // 0xFF : represent a cell not assigned to a plate yet
	float_t      elevation = 0.1f;
	float_t      lat = 0.0f;                       // radians [-PI/2, PI/2]
	float_t      lon = 0.f;                        // in radiand [-PI, PI)
};







//                                     brass,              brown,            burnt sienna,     camel,                chocolate,         dark brown, 
//static const QColor plateColors[12] = { QColor(225,193,110),QColor(165,42,42),QColor(233,116,81),QColor(193,154,107), QColor(123, 63,0), QColor(92,64,51),
//                                     fawn,               khaki,              maroon,         nude,                olive green,      Tuscan Red
//																		    QColor(229,170,112),QColor(240,230,140),QColor(128,0,0),QColor(242,210,189), QColor(128,128,0),QColor(124,48,48) };




#endif
