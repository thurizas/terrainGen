/*
 * file     : hexagon.cpp
 *
 * abstract : Contains the implementation of the hexagon class.  This class maintains a set of vertices for a
 *            hexagon.  In a regular hexagon, the six internal angles are each 120 degrees.  Knowing just the
 *            side length, s, we can calculate some additional properties.  Consider the following diagram (in
 *            poor ascii art):
 *
 *                                    a = 2*r                            a = s + 2r
 *                              __   |--------|                        |---------------|
 *                              |       /\
 *                              |      /0 \                     _          0_ _ _ _ 1
 *                              |     /    \                    |          /       \
 *                              |    /      \                   |         /         \
 *                              |    |5     1|                  |        /           \
 *                  s + 2*h = b |    |   *   |                  |       <5     *     2>|
 *                              |    |4     2|                  |        \           / |
 *                              |    \      /|                  |         \         /  | h = s * cos(30)
 *                              |	    \    / |  h = s*sin(30)   |          \_ _ _ _/___|
 *                              |	     \3 /  |                  -          4      3  r = s * sin(30)
 *                              __	  	\/___|
 *                                        r = s*cos(30)
 *
 *            So, if we have the center of a hexagon (represented by the asterics) at {x,y} we can calculate
 *            the location of the six verticies thusly.  Vertex[0] is the top vertex and they are numberd
 *            going clockwise.  Both forms have clockwise winding
 *
 *                            upright
 *            vertex[0] = {x    , y - ((s/2) + h)}                   {x - (s/2)     , y - h}
 *            vertex[1] = {x + r, y - (s/2)      }                   {x + (s/2)     , y - h}
 *            vertex[2] = {x + r, y + (s/2)      }                   {x + ((s/2) + r, y    }
 *            vertex[3] = {x    , y + ((s/2) + h)}                   {x + (s/2)     , y + h}
 *            vertex[4] = {x - r, y - (s/2)      }                   {x - (s/2)     , y + h}
 *            vertex[5] = {x - r, y - ((s/2) + h)}                   {x - ((s/2) + r, y    }
 *
 *            We need some useful constants (to cut down on the math overhead
 *
 *            pi      = 3.14159265
 *            sin(30) = 0.5
 *            cos(30) = 0.86602540
 *
 * neighbor calculations:  for a grid in this format (our default verticle layout):
 *
 *
 *       /\      /\
 *      /  \    /  \
 *     /    \  /    \
 *    /      \/      \      the neighbors are given by:
 *   |       |       |                          for moves:  NW     ,  NE,   ,   W    , E     , SW     ,   SE
 *   |       |       |            for an even row number : [-1, -1], [-1,  0], [0, -1], [0, 1], [1, -1], [1, 0]
 *   |  (0,0)| (0,1) |            for an odd row number  : [-1, 0] , [-1, +1], [0, -1], [0, 1]. [1, 0],  [1, 1]
 *   |       |       |
 *   \      / \      /
 *    \    /   \    /
 *     \  /     \  /
 *      \/       \/
 *      |         |
 *      |  (1,0)  |
 *      |         |
 *      |         |
 *       \       /
 *        \     /
 *         \   /
 *          \ /


 */
#include "constants.h"
#include "hexagon.h"
#include "utility.h"

#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QFontMetrics>

uint32_t CHexagon::m_curNdx = 0;

// x coordinate of center, y coordinate of center, length of a side, orientation.
CHexagon::CHexagon(float _x, float _y, float _s, int _m) : m_ndx(getNextNdx()), m_nX(_x), m_nY(_y), m_nSide(_s), m_pLabel(NULL), m_nMode(_m), m_nState(0)
{
    calcVertices();
}


const CHexagon& CHexagon::operator=(const CHexagon& rhs)
{
    if (this != &rhs)
    {
        this->m_nX = rhs.m_nX;
        this->m_nY = rhs.m_nY;
        this->m_nSide = rhs.m_nSide;
        this->m_nMode = rhs.m_nMode;
        this->m_nState = rhs.m_nState;       // true: filled, false: empty
        this->m_pLabel = nullptr;

        if (nullptr != rhs.m_pLabel)
        {
            uint32_t len = strlen(rhs.m_pLabel);
            this->m_pLabel = new char[len + 1];
            strcpy(this->m_pLabel, rhs.m_pLabel);
        }

        this->m_verticies = rhs.m_verticies;
    }

    return *this;
}



CHexagon::~CHexagon()
{
    if (NULL != m_pLabel)
    {
        delete[] m_pLabel;
        m_pLabel = NULL;
    }
}


// note: we print out column, row format to be consistent with https://www.redblobgames.com/grids/hexagons/
void CHexagon::makeLabel(int ndx, int row, int col)
{
    m_pLabel = new char[13];
    if (m_nMode == CHexagon::VERTICAL)
        sprintf(m_pLabel, "%04d\n%03d,%03d", ndx, col, row);
    else if (m_nMode == CHexagon::HORIZONTAL)
        sprintf(m_pLabel, "%04d\n%03d,%03d", ndx, col, row);
    else
        fprintf(stderr, "unknown mode\n");
}

void CHexagon::makeLabel(int ndx)
{
    m_pLabel = new char[8];
    sprintf(m_pLabel, "%04d", ndx);
}


void CHexagon::calcVertices()
{
    float nHeigth = m_nSide * sin30;                                    // heigth of the triange in comments
    float nLength = m_nSide * cos30;                                    // base of triange in comments
    float nHalfSide = m_nSide / 2;                                      // half of a side.
    float nHalfHeigth = nHalfSide + nHeigth;                            // half the heigth of the hex

    if (m_nMode == VERTICAL/*HORIZONTAL*/)
    {
        m_verticies.push_back(QPointF(m_nX, m_nY - nHalfHeigth));
        m_verticies.push_back(QPointF(m_nX + nLength, m_nY - nHalfSide));
        m_verticies.push_back(QPointF(m_nX + nLength, m_nY + nHalfSide));
        m_verticies.push_back(QPointF(m_nX, m_nY + nHalfHeigth));
        m_verticies.push_back(QPointF(m_nX - nLength, m_nY + nHalfSide));
        m_verticies.push_back(QPointF(m_nX - nLength, m_nY - nHalfSide));
        m_verticies.push_back(QPointF(m_nX, m_nY - nHalfHeigth));              // need this point to be able to close the figure
    }
    else if(m_nMode == HORIZONTAL/*VERTICAL*/)
    {
        m_verticies.push_back(QPointF(m_nX - nHalfSide, m_nY - nLength));
        m_verticies.push_back(QPointF(m_nX + nHalfSide, m_nY - nLength));
        m_verticies.push_back(QPointF(m_nX + (nHalfSide + nHeigth), m_nY));
        m_verticies.push_back(QPointF(m_nX + nHalfSide, m_nY + nLength));
        m_verticies.push_back(QPointF(m_nX - nHalfSide, m_nY + nLength));
        m_verticies.push_back(QPointF(m_nX - (nHalfSide + nHeigth), m_nY));
        m_verticies.push_back(QPointF(m_nX - nHalfSide, m_nY - nLength));      // need this point to be able to close the figure
    }
    else
    {
        fprintf(stderr, "unknown mode");
    }
}


void CHexagon::draw(QGraphicsScene* pScene)
{
    QPolygonF      polygon;
    Qt::BrushStyle style = Qt::NoBrush;           // set to a hollow pattern
    QColor         color = Qt::black;

    if ((m_nState & bDispCenter) == bDispCenter)     // draw center point if requested....
    {
        QGraphicsEllipseItem* pCenter = new QGraphicsEllipseItem(m_nX - 1, m_nY - 1, 2, 2, nullptr);
        pCenter->setBrush(Qt::red);
        pScene->addItem(pCenter);


    }

    if ((m_nState & bDispIndex) == bDispIndex)
    {
      // calculate space for a four digit number
      QFont  font; 
      QFontMetrics fm(font);
      QString test("wwww");
      QRect bb = fm.boundingRect(test);        // get bounding box for test string, centered at origin
    
      QRectF bbox = QRectF(m_nX - (bb.width() / 2), m_nY - (bb.height() / 2), bb.width(), bb.height());   //xlate bounding box to hex center
      QString label = QString("%1").arg(m_ndx, 4, 10, QChar('0'));
      QGraphicsSimpleTextItem*  pText = new QGraphicsSimpleTextItem();
      pText->setText(label);
      pText->setPos(pText->pos() + QPoint(m_nX - (3*bb.width()/8), m_nY- (4*bb.height()/8)));
      pScene->addItem(pText);
    }

    if ((m_nState & bColor) == bColor)         // check for color bit
    {
      color = getColor();
    }

    if ((m_nState & bFilled) == bFilled)        // check for filled flag...
    {
        style = Qt::SolidPattern;
    }

    QPen  pen(color);
    QBrush brush(color, style);

    for(int ndx = 0; ndx < m_verticies.size(); ndx++)
        polygon.append(m_verticies.at(ndx));

    pScene->addPolygon(polygon, pen, brush);
   
}


/**************************************************************************************************
 * Function: 
 *
 * Abstract: For simplisity we translate the hexagon so it is centered at the origin, this is done
 *           by the equations
 *                  dx = (pt.x() - m_nX)/radius
 *                  dy = (pt.y() - m_nY)/radius
 *           where radius is the distance from the center to any vertex.  This means that the point
 *           is (dx,dy) and the center is at (0,0) and the radius is one.  We check to see that the
 *           center and the point are on the same side of each edge.
 * 
 *           For a vertically orientated hexagon with side length of 1, the two vertical sides are 
 * 
 *                   vertical orientated                   horizontal orientated
 * verticies |  x                |  y             |  verticies |  x                |  y                |
 *         0 | c_x + r*\cos(30)  | c_y +\sin(60)  |          0 | c_x + r*\cos(120) | c_y + r*\sin(120) |
 *           |   = \sqrt{3}/2    |   = 1/2        |            |   = -0.5*r        |   = r*\sqrt(3)/2  |
 *         1 | c_x + r*\cos(330) | c_y +\sin(330) |          1 | c_x + r*\cos(60)  | c_y + r*\sin(60)  |
 *           |   = \sqrt{3}/2    |   = -1/        |            |   = 0.5*r         |   = r*\sqrt(3)/2  |
 *         2 | c_x + r*\cos(270) | c_y +\sin(270) |          2 | c_x + r*\cos(0)   | c_y + r*\sin(0)   | 
 *           |   = 0             |   = -r         |            |   = r             |   = 0             |
 *         3 | c_x + r*\cos(210) | c_y +\sin(210) |          3 | c_x + r*\cos(300) | c_y + r*\sin(300) |
 *           |   = -\sqrt{3}/2   |   -1/2         |            |   = 0.5*r         |   = -r*\sqrt(3)/2 |
 *         4 | c_x + r*\cos(150) | c_y +\sin(150) |          4 | c_x + r*\cos(240) | c_y + r*\sin(240) |
 *           |   = -\sqrt{3}/2   |    1/2         |            |   = -0.5*r        |   = -r*\sqrt(3)/2 |
 *         5 | c_x + r*\cos(90)  | c_y +\sin(90)  |          5 | c_x + r*\cos(180) | c_y + r*sin(180)  |
 *           |    = 0            |   = r          |            |   = -r            |   = 0             |
 *                                      
 *                             /\                                                                             
 *                            /0 \                                            0_ _ _ _ 1
 *                           /    \                                           /       \
 *                          / 5   1\                                         /         \
 *                          |      |                                        /           \
 *                          |   *  |                                       <5     *     2>
 *                          |4    2|                                        \           / 
 *                          \      /                                         \         /  
 *                           \    /                                           \_ _ _ _/
 *                            \3 /                                             4      3  
 *                    	       \/                                                                                
 *                                                                    
 *  equations of the sides:
 * 
 *  for the two vertical sides x = \sqrt{3}/2 and x = -\sqrt{3}\2 | horizontal side   y = \frac{\sqrt{3}}{2} and y = -\frac{\sqrt{3}}{2}
 *  for the upper right side   y = -\frac{x}{\sqrt{3}} + 1        |                   y = -x \sqrt{3} + \sqrt{3}
 *  for the upper left side    y = \frac{x}{\sqrt{3}} + 1         |                   y = x \sqrt{3} + \sqrt{3}
 *  for the bottom left side   y = -\frac{x}{\sqrt{3}} - 1        |                   y   x \sqrt{3} - \sqrt{3}
 *  for the bottom right side  y = \frax{x}{\sqrt{3}} - 1         |                   y = x \sqrt{3} - \sqrt{3}
 * 
 *consider the point (0.5, 0.5) w.r.t the top left side (with vertical orientation)
 *  for center                         test point                                  
 *   0 = 0\sqrt(3) + 1              0.5 = 0.5\sqrt(3) + 1                          
 *   0 < 1                          0.5 < 0.8660 +1 = 1.8669
 *   in both cases the right side is greater, thus interior point                  
 * 
 *consider the point (1.0,1.0) w.r.t. the top left side... 
 *  for center                         test point                                  
 *  0 = 0\sqrt(3) + 1               1.0 = 1.0\sqrt(3) + 1
 *  0 < 1                           1.0 < 1.7320 + 1
 * 
 *consider the point (0.5, 0.5) w.r.t the top left side (with horizontal orientation) 
 *  for center                         test point                                   
 *   0 = 0\sqrt(3) + \sqrt(3)          0.5 = 0.5\sqrt(3) + \sqrt(3)
 *   0 < 1.7320                        0.5 < 0.8660 +1.7320
 *   in both cases the right side is greater, thus interior point                   
 *  
 *consider the point (1.0,1.0) w.r.t. the top left side...           
 *  for center                         test point                                  
 *  0 = 0\sqrt(3) + \sqrt(3)        1.0 = 1.0\sqrt(3) + 1.7320
 *  0 < 1.7320                      1.0 < 1.7320 + 1.732
 * 
 *  see: https://stackoverflow.com/questions/5193331/is-a-point-inside-regular-hexagon
 * 
 * Input   :
 * 
 * Returns :
 *
 * Written : Feb 2026 (gkhuber) 
 *************************************************************************************************/
bool CHexagon::contains(QPointF pt)
{
  bool    res = false;
  double  min_x, max_x, min_y, max_y;

  float radius = sqrt((m_nX - m_verticies.at(0).x()) * (m_nX - m_verticies.at(0).x()) + (m_nY - m_verticies.at(0).y()) * (m_nY - m_verticies.at(0).y()));

  float dx = (pt.x() - m_nX) / radius;
  float dy = (pt.y() - m_nY) / radius;

  if (m_nMode == CHexagon::HORIZONTAL)
  {
    min_x = m_verticies.at(0).x();                 // top left
    min_y = m_verticies.at(0).y();
    max_x = m_verticies.at(3).x();                 // bottom right
    max_y = m_verticies.at(3).y();

    if ((min_x < pt.x()) && (pt.x() <= max_x) && (min_y < pt.y()) && (pt.y() <= max_y))            // point in internal square ??
    {
      res =  true;
    }
    
    // build bounding box
    min_x = m_verticies.at(5).x();
    max_x = m_verticies.at(2).x();
    min_y = m_verticies.at(0).y();
    max_y = m_verticies.at(3).y();

    if (!((min_x < pt.x()) && (pt.x() <= max_x) && (min_y < pt.y()) && (pt.y() <= max_y)))           // point not in bounding box??
    {
      res =  false;
    }
    else
    {
      // check diagonals
      int8_t tlOrient = orient(m_verticies.at(0), m_verticies.at(5), pt);
      int8_t blOrient = orient(m_verticies.at(5), m_verticies.at(4), pt); 
      int8_t brOrient = orient(m_verticies.at(3), m_verticies.at(2), pt); 
      int8_t trOrient = orient(m_verticies.at(2), m_verticies.at(1), pt); 

      if ((dir::LEFT == tlOrient) && (dir::LEFT == blOrient) && (dir::LEFT == brOrient) && (dir::LEFT == trOrient))
      {
        res = true;
      }
      else
      {
        res = false;
      }
    }
  }
  else if (m_nMode == CHexagon::VERTICAL)
  {
    min_x = m_verticies.at(5).x();                            // top left
    min_y = m_verticies.at(5).y();
    max_x = m_verticies.at(2).x();                            // bottom right
    max_y = m_verticies.at(2).y();

    if ((min_x < pt.x()) && (pt.x() <= max_x) && (min_y < pt.y()) && (pt.y() <= max_y))            // point in internal square ??
    {
      res = true;
    }

    // build bounding box
    min_x = m_verticies.at(4).x();
    max_x = m_verticies.at(1).x();
    min_y = m_verticies.at(0).y();
    max_y = m_verticies.at(3).y();

    if (!((min_x < pt.x()) && (pt.x() <= max_x) && (min_y < pt.y()) && (pt.y() <= max_y)))           // point not in bounding box??
    {
      res = false;
    }
    else
    {
      // check diagonals
      int8_t trOrient = orient(m_verticies.at(1), m_verticies.at(0), pt);
      int8_t tlOrient = orient(m_verticies.at(0), m_verticies.at(5), pt);
      int8_t blOrient = orient(m_verticies.at(4), m_verticies.at(3), pt);
      int8_t brOrient = orient(m_verticies.at(3), m_verticies.at(2), pt);

      if ((dir::LEFT == trOrient) && (dir::LEFT == tlOrient) && (dir::LEFT == blOrient) && (dir::LEFT == brOrient))
      {
        res = true;
      }
      else
      {
        res = false;
      }
    }
  }

  return res;
}



