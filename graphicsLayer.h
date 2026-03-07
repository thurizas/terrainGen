#ifndef _graphicslayer_h_
#define _graphicslayer_h_

#include <QGraphicsItem>

class graphicsLayer : public QGraphicsItem
{
public:
  graphicsLayer(QGraphicsItem* p = nullptr) : QGraphicsItem(p)
  { }

  virtual QRectF boundingRect() const
  {
    return QRectF(0.0, 0.0, 0.0, 0.0);
  }

  virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
  {

  }

};

#endif

