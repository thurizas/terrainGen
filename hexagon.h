#ifndef _hexagon_h_
#define _hexagon_h_

#include "constants.h"

#include <QGraphicsItem>
#include <QGraphicsItemGroup>
#include <QPen>
#include <QBrush>

#include "graphicsLayer.h"


class hexagon : public QGraphicsItemGroup
{
public:
  enum orien: std::uint8_t {UNKNOWN = 0, VERTICAL = 1, HORIZONTAL = 2};
  enum style: std::uint8_t {NONE=0, HOLLOW=bHollow, SOLID=bFilled};

  hexagon(QPointF center, struct imageProps*, QGraphicsItem* p = nullptr);
  QRectF   boundingRect(); 
  
  
  static uint32_t getIndex() { return s_Ndx++; }
  uint32_t getId() { return m_id; }

  
  QPointF  getCenter() { return m_center; }
  double_t getRadius() { return m_side/sqrt3; }
  double_t getSide() { return m_side;}
  QString  getLabel(QRectF* pbbox = nullptr);
  

  bool contains(QPointF pt);

  void    setStyle(uint8_t);
  uint8_t getStyle() { return m_style; }

  void setColor(QColor c);
  QColor getColor() { return m_borderColor; }

  QGraphicsItemGroup* getGroup() { return this; }
  

private:
  static uint32_t                 s_Ndx;
  uint32_t                        m_id;
  std::array<QPointF, 6>          m_vertices;
  QPointF                         m_center;             
  double_t                        m_radius;              // radius of circumscribed circle
  double_t                        m_side;                // length of a side of the hexagon
  uint8_t                         m_orient;
  uint8_t                         m_style;
  QColor                          m_borderColor;
  QPen                            m_pen;
  QBrush                          m_brush;
  QRectF                          m_bbox;
  QGraphicsPolygonItem*           m_hex;
};

#endif

