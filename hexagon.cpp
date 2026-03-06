#include "hexagon.h"
#include "constants.h"
#include "logger.h"
#include "utility.h"

#include <QGraphicsItem>
#include <QGraphicsItemGroup>
#include <QPen>
#include <QFontMetrics>
#include <QPainter>


enum orien : std::uint8_t { UNKNOWN = 0, VERTICAL = 1, HORIZONTAL = 1 };
enum style : std::uint8_t { NONE = 0, SOLID = 1, HOLLOW = 2 };

uint32_t hexagon::s_Ndx = 0;

/*********************************************************************************************************************
 * Function: 
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
 * Input   :
 *
 * Returns :
 *
 * Written : () 
 ********************************************************************************************************************/
hexagon::hexagon(QPointF c, struct imageProps* props, QGraphicsItem* p) : QGraphicsItemGroup(p), m_center(c), m_id(hexagon::getIndex()), m_size(props->hexagonSize), m_orient(props->hexagonOrient), m_style(hexagon::style::HOLLOW), m_borderColor(Qt::black)
{
  // build the array of verticies in CCW winding order
  if (m_orient == hexagon::orien::VERTICAL)
  {
    double_t  height = m_size * sin30;
    double_t  radius = m_size * cos30;

    m_vertices[0] = QPointF(m_center.x(), m_center.y() - ((m_size / 2) + height)); // ( 51.96,  0.0 )
    m_vertices[1] = QPointF(m_center.x() + radius, m_center.y() - (m_size / 2));   // (103.92, 30.00)
    m_vertices[2] = QPointF(m_center.x() + radius, m_center.y() + (m_size / 2));   // (103.92, 90.00)
    m_vertices[3] = QPointF(m_center.x(), m_center.y() + ((m_size / 2) + height)); // (51.96, 120.00)
    m_vertices[4] = QPointF(m_center.x() - radius, m_center.y() + (m_size / 2));   // (  0.00, 30.0 )
    m_vertices[5] = QPointF(m_center.x() - radius, m_center.y() - (m_size / 2));   // (  0.00, 90.0 )

    m_bbox.setTopLeft(QPointF(m_vertices[5].x(), m_vertices[0].y()));
    m_bbox.setBottomRight(QPointF(m_vertices[1].x(), m_vertices[3].y()));
    m_bbox.setHeight(m_vertices[3].y() - m_vertices[0].y());
    m_bbox.setWidth(m_vertices[1].x() - m_vertices[5].x());
  }
  else
  {
    double_t height = m_size * cos30;
    double_t radius = m_size * sin30;

    m_vertices[0] = QPointF(m_center.x() - (m_size / 2)         , m_center.y() - height);  // ( 30,  0.0)
    m_vertices[1] = QPointF(m_center.x() + (m_size / 2)         , m_center.y() - height);  // ( 90,  0.0)
    m_vertices[2] = QPointF(m_center.x() + (m_size / 2) + radius, m_center.y());           // (120,  51.96)
    m_vertices[3] = QPointF(m_center.x() + (m_size / 2)         , m_center.y() + height);  // ( 90, 103.92)
    m_vertices[4] = QPointF(m_center.x() - (m_size / 2)         , m_center.y() + height);  // ( 30, 103.92)
    m_vertices[5] = QPointF(m_center.x() - (m_size / 2) - radius, m_center.y());           // (  0,  51.96)

    m_bbox.setTopLeft(QPointF(m_vertices[5].x(), m_vertices[0].y()));
    m_bbox.setBottomRight(QPointF(m_vertices[3].x(), m_vertices[2].y()));
    m_bbox.setHeight(m_vertices[4].y() - m_vertices[0].y());
    m_bbox.setWidth(m_vertices[2].x() - m_vertices[5].x());
  }

  // build a QPolygon to hold verticies, addind first to end to force a close polygon
  QPolygonF h;
  for (uint32_t ndx = 0; ndx <= 6; ndx++)
  {
    h << m_vertices[ndx % 6];
  }
  
  //set up pen & brush characteristics
  m_brush.setColor(m_borderColor);
  if ((m_style & hexagon::style::HOLLOW) == hexagon::style::HOLLOW)
    m_brush.setStyle(Qt::NoBrush);
  else if ((m_style & hexagon::style::SOLID) == hexagon::style::SOLID)
    m_brush.setStyle(Qt::SolidPattern);


  m_pen.setColor(m_borderColor);
  m_pen.setWidth(1);
  //m_pen.setBrush(m_brush);

  m_hex = new QGraphicsPolygonItem(h);
  m_hex->setPen(m_pen);

  // add edges to the QGraphicsItemGroup.
  addToGroup(m_hex);
}
  
QRectF hexagon::boundingRect()
{
  if (m_orient == hexagon::orien::VERTICAL)
  {

  }
  else if (m_orient == hexagon::orien::HORIZONTAL)
  {

  }
  else
  {
    CLogger::getInstance()->outMsg(cmdLine, CLogger::level::ERR, "hexagon::boundingRect -- orientation is unset");
  }

  return m_bbox;
}

//void hexagon::paint(QPainter* painter, const QStyleOptionGraphicsItem* opt, QWidget* widget)
//{
//  painter->setPen(m_pen);
//  painter->setBrush(m_brush);
//
//  QGraphicsItemGroup::paint(painter, opt, widget);                         // let base class do all of the work
//
//}


QPointF  hexagon::getCenter()
{
  return m_center;
}


double_t hexagon::getSize()
{
  return m_size;
}

QString hexagon::getLabel(QRectF* pbbox)
{
  QString ret = "";

  ret = QString("%1").arg((int)m_id, 4);

  if (pbbox != nullptr)
  {
    QFont font;
    QFontMetrics fm(font);

    pbbox->setLeft(0);
    pbbox->setTop(0);
    pbbox->setWidth(fm.horizontalAdvance("4444"));
    pbbox->setHeight(fm.height());
  }

  return ret;
}

/**********************************************************************************************************************
 * Function: contains
 *
 * Abstract: this function determines if the hexagon contains the given point,  this test is performed in three steps,
 *           (1) first is the point located in or on the inner square - if so return true
 *           (2) second, is the point contained in the outer bounding box - if not return false
 *           (3) third, determine orientation to four diagonal edges - if all are left return true.
 *           These tests are different based on the orientation, see below
 *                
 *                    /\                                
 *                   /0 \                                   0_ _ _ _ 1
 *                  /    \                                  /       \
 *                 /      \                                /         \
 *                 |5     1|                              /           \
 *                 |   *   |                             <5     *     2>
 *                 |4     2|                              \           / 
 *                 \      /|                               \         /  
 *                  \    / |                                \_ _ _ _/
 *                   \3 /  |                                4      3 
 *                  	\/___|                            
 *                                                      
 *          inner rectangle: v5 -> v2                       v0 -> v3
 *          bounding box     (v5_x, v0_y) -> (v1_x, v3_y)   (v5_x, v0_y) -> (v2_x, v3_y)
 *          edges to check   e(v0, v5), e(v4, v3)           e(v0, v5), e(v5, v4)
 *                           e(v3, v2), e(v1, v0)           e(v3, v2), e(v2, v1)
 * 
 * Input   : pt -- [in] QPointF object to check membership in internal and border points.
 *
 * Returns : boolean, true if point belongs to the hexagon false otherwise
 *
 * Written : Mar 2026 (gkhuber) 
 *********************************************************************************************************************/
bool hexagon::contains(QPointF pt)
{
  bool inHex = false;

  if (m_orient == hexagon::orien::VERTICAL)
  {
    if((m_vertices.at(5).x() <= pt.x()) && (pt.x() < m_vertices.at(2).x()) &&   // check membership in inner rectangle
      (m_vertices.at(5).y() < pt.y()) && (pt.y() < m_vertices.at(2).y()))
    {
      inHex = true;
    }
    else
    {
      if (!((m_vertices.at(5).x() <= pt.x()) && (pt.x() < m_vertices.at(1).x()) &&  // check memberhip in bounding box
        (m_vertices.at(0).y() <= pt.y()) && (pt.y() < m_vertices.at(3).y())))
      {
        inHex = false;
      }
      else
      {
        int8_t o1 = orient(m_vertices.at(0), m_vertices.at(5), pt);
        int8_t o2 = orient(m_vertices.at(4), m_vertices.at(3), pt);
        int8_t o3 = orient(m_vertices.at(3), m_vertices.at(2), pt);
        int8_t o4 = orient(m_vertices.at(1), m_vertices.at(0), pt);

        if ((o1 == dir::LEFT) && (o2 == dir::LEFT) && (o3 == dir::LEFT) && (o4 == dir::LEFT))
        {
          inHex = true;
        }
      }
    }
  }  // end hexagon::orien::VERTICAL checks
  else if (m_orient == hexagon::orien::HORIZONTAL)
  {
    if ((m_vertices.at(0).x() <= pt.x()) && (pt.x() < m_vertices.at(3).x()) &&   // check membership in inner rectangle
      (m_vertices.at(0).y() < pt.y()) && (pt.y() < m_vertices.at(3).y()))
    {
      inHex = true;
    }
    else
    {
      if (!((m_vertices.at(5).x() <= pt.x()) && (pt.x() < m_vertices.at(2).x()) &&  // check memberhip in bounding box
        (m_vertices.at(0).y() <= pt.y()) && (pt.y() < m_vertices.at(3).y())))
      {
        inHex = false;
      }
      else
      {
        int8_t o1 = orient(m_vertices.at(0), m_vertices.at(5), pt);
        int8_t o2 = orient(m_vertices.at(5), m_vertices.at(4), pt);
        int8_t o3 = orient(m_vertices.at(3), m_vertices.at(2), pt);
        int8_t o4 = orient(m_vertices.at(2), m_vertices.at(1), pt);

        if ((o1 == dir::LEFT) && (o2 == dir::LEFT) && (o3 == dir::LEFT) && (o4 == dir::LEFT))
        {
          inHex = true;
        }
      }
    }
  }
  else
  {
    CLogger::getInstance()->outMsg(cmdLine, CLogger::level::WARNING, "unknown or illegal orientation");
  }

  return inHex;
}

void hexagon::setStyle(uint8_t s)
{
  m_style = s;

  m_brush.setColor(m_borderColor);
  if ((m_style & hexagon::style::HOLLOW) == hexagon::style::HOLLOW)
    m_brush.setStyle(Qt::NoBrush);
  else if ((m_style & hexagon::style::SOLID) == hexagon::style::SOLID)
    m_brush.setStyle(Qt::SolidPattern);

  m_hex->setBrush(m_brush);
  m_hex->setPen(m_pen);
}

void hexagon::setColor(QColor c)
{
  m_borderColor = c;

  m_pen.setColor(m_borderColor);
  m_pen.setWidth(1);
}
