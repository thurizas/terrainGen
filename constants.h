#ifndef _constants_h_
#define _constants_h_

#include <cstdint>
#include <QColor>

static const uint8_t MAJOR = 1;
static const uint8_t MINOR = 0;
static const uint16_t PATCH = 0;


static const uint32_t scrWidth = 800;
static const uint32_t scrHeight = 600;
static const uint32_t mbarHeight = 21;

static const uint8_t nbrHarmonics = 3;

static const uint8_t bFilled = 0x01;     //b0000 0001
static const uint8_t bColor = 0x02;      //b0000 0010
static const uint8_t bDispCenter = 0x04; //b0000 0100
static const uint8_t bDispIndex = 0x08;  //b0000 1000



static const float epsilion = 1E-5;
static const float     pi = 3.1415926535897932384626433832795;
static const float  sqrt3 = 1.7320508075688772935274463415059;
static const double sin30 = 0.5000000000000000000000000000000;      // sin30 = 1/2
static const double cos30 = 0.8660254037844386467637231707529;      // cos30 = sqrt(3)/2
static const double sin60 = 0.8660254037844386467637231707529;      // sin60 = sqrt(3)/2
static const double cos60 = 0.5000000000000000000000000000000;      // cos30 = 1/2

struct imageProps
{
	uint64_t       imageWidth;
	uint64_t       imageHeight;
	uint64_t       hexagonSize;
	uint8_t        hexagonOrient;
	uint8_t        hexagonProps;
	uint32_t       cntPlates;

	float          amplitude[nbrHarmonics];
	float          frequency[nbrHarmonics];
};

//                                     brass,              brown,            burnt sienna,     camel,                chocolate,         dark brown, 
static const QColor plateColors[12] = { QColor(225,193,110),QColor(165,42,42),QColor(233,116,81),QColor(193,154,107), QColor(123, 63,0), QColor(92,64,51),
//                                     fawn,               khaki,              maroon,         nude,                olive green,      Tuscan Red
																		    QColor(229,170,112),QColor(240,230,140),QColor(128,0,0),QColor(242,210,189), QColor(128,128,0),QColor(124,48,48) };

typedef struct plates
{
	uint32_t ndx;
	double_t center_x;
	double_t center_y;
	QColor   color;

	std::vector<uint32_t> vec;             // index of hexagons on boundary
} platesT, * pPlatesT;

#endif
